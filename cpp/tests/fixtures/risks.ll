declare i32 @external(i32)
declare i32 @rand()
declare i32 @llvm.abs.i32(i32, i1 immarg)
declare i32 @llvm.smin.i32(i32, i32)
declare i32 @llvm.smax.i32(i32, i32)
declare i32 @llvm.umin.i32(i32, i32)
declare i32 @llvm.umax.i32(i32, i32)

define i32 @intrinsic_abs(i32 %x) {
  %a = call i32 @llvm.abs.i32(i32 %x, i1 true)
  %ok = icmp sle i32 %a, 100
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add nsw i32 %a, 1
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @intrinsic_minmax(i32 %x) {
  %low = call i32 @llvm.smax.i32(i32 %x, i32 0)
  %high = call i32 @llvm.smin.i32(i32 %low, i32 100)
  %sum = add nsw i32 %high, 1
  ret i32 %sum
}
define i32 @intrinsic_uminmax(i32 %x) {
  %high = call i32 @llvm.umin.i32(i32 %x, i32 100)
  %low = call i32 @llvm.umax.i32(i32 %high, i32 0)
  %sum = add nuw i32 %low, 1
  ret i32 %sum
}
define i32 @rand_small() {
  %r = call i32 @rand()
  %sum = add nsw i32 %r, 2147450879
  ret i32 %sum
}
define i32 @return_source(i32 %ignored) {
  ret i32 2147483647
}
define i32 @return_caller() {
  %r = call i32 @return_source(i32 0)
  %sum = add nsw i32 %r, 1
  ret i32 %sum
}

define i32 @raw(i32 %x) {
  %sum = add nsw nuw i32 %x, 8
  ret i32 %sum
}
define i64 @raw64(i64 %x) {
  %sum = add nsw nuw i64 %x, 8
  ret i64 %sum
}
define i32 @guarded(i32 %x) {
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add nsw nuw i32 %x, 8
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @late_guard(i32 %x) {
  %sum = add nsw nuw i32 %x, 8
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %exit
safe:
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @guarded_parent(i32 %x) {
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %exit
safe:
  %r = call i32 @raw(i32 %x)
  ret i32 %r
exit:
  ret i32 0
}
define i32 @subtract(i32 %x, i32 %y) {
  %ok = icmp uge i32 %x, %y
  br i1 %ok, label %safe, label %exit
safe:
  %r = sub i32 %x, %y
  ret i32 %r
exit:
  ret i32 0
}
define i32 @multiply(i32 %x, i32 %y) {
  %r = mul nsw nuw i32 %x, %y
  ret i32 %r
}
define i64 @divide(i64 %x, i64 %y) {
  %r = sdiv i64 %x, %y
  ret i64 %r
}
define i64 @remainder(i64 %x, i64 %y) {
  %r = srem i64 %x, %y
  ret i64 %r
}
define i64 @safe_divide(i64 %x, i64 %y) {
  %zero = icmp eq i64 %y, 0
  %min = icmp eq i64 %x, -9223372036854775808
  %neg = icmp eq i64 %y, -1
  %overflow = and i1 %min, %neg
  %bad = or i1 %zero, %overflow
  br i1 %bad, label %exit, label %safe
safe:
  %r = sdiv i64 %x, %y
  ret i64 %r
exit:
  ret i64 0
}
define i32 @memory(i32 %x, ptr %p) {
  %v = load i32, ptr %p
  %sum = add i32 %x, %v
  ret i32 %sum
}
define i32 @guarded_memory(i32 %x, ptr %p) {
  %v = load i32, ptr %p
  %bound_x = icmp ule i32 %x, 8
  %bound_v = icmp ule i32 %v, 1000
  %ok = and i1 %bound_x, %bound_v
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add i32 %v, %x
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @independent_memory(i32 %x, ptr %p, ptr %q) {
entry:
  %unrelated = load i32, ptr %p
  store i32 %unrelated, ptr %q
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add i32 %x, 8
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @control_only_memory(i32 %x, ptr %p) {
entry:
  %v = load i32, ptr %p
  %gate = icmp ne i32 %v, 0
  br i1 %gate, label %check, label %exit
check:
  %bounded = icmp ule i32 %x, 1000
  br i1 %bounded, label %risk, label %exit
risk:
  %sum = add i32 %x, 8
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @mixed_modeled_unknown(i32 %x, ptr %p) {
entry:
  %is_zero = icmp eq i32 %x, 0
  br i1 %is_zero, label %unknown, label %modeled
unknown:
  %v = load i32, ptr %p
  br label %merge
modeled:
  br label %merge
merge:
  %input = phi i32 [ %v, %unknown ], [ %x, %modeled ]
  %sum = add i32 %input, 8
  ret i32 %sum
}
define i32 @boundary_return(i32 %x) {
  ret i32 0
}
define i32 @outside_return(i32 %x) {
entry:
  %v = call i32 @boundary_return(i32 %x)
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add i32 %x, %v
  ret i32 %sum
exit:
  ret i32 0
}
define void @boundary_write(ptr %p, i32 %x) {
  store i32 %x, ptr %p
  ret void
}
define i32 @outside_pointer(i32 %x) {
entry:
  %local = alloca i32
  store i32 0, ptr %local
  call void @boundary_write(ptr %local, i32 %x)
  %v = load i32, ptr %local
  %ok = icmp ule i32 %x, 8
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add i32 %x, %v
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @indirect_value(i32 %x, ptr %callee) {
  %v = call i32 %callee(i32 %x)
  %sum = add i32 %x, %v
  ret i32 %sum
}
define i32 @unsupported_value(i32 %x) {
  %v = freeze i32 %x
  %sum = add i32 %v, 8
  ret i32 %sum
}
define i32 @loop_cut(i32 %x) {
entry:
  br label %loop
loop:
  %i = phi i32 [ %x, %entry ], [ %next, %loop ]
  %next = xor i32 %i, 1
  %again = icmp eq i32 %next, 0
  br i1 %again, label %loop, label %exit
exit:
  %sum = add i32 %next, 8
  ret i32 %sum
}
define i32 @unknown_call(i32 %x) {
  %v = call i32 @external(i32 %x)
  %sum = add i32 %x, %v
  ret i32 %sum
}
define i32 @rand_risk(i32 %x) {
  %r = call i32 @rand()
  %sum = add i32 %x, %r
  ret i32 %sum
}
define i32 @rand_guarded(i32 %x) {
entry:
  %r = call i32 @rand()
  %masked = and i32 %r, 7
  %ok = icmp ule i32 %x, 8
  br i1 %ok, label %safe, label %exit
safe:
  %sum = add i32 %masked, %x
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @closed_sink(i32 %x) {
  %sum = add i32 %x, 8
  ret i32 %sum
}
define void @closed_source() {
  %r = call i32 @rand()
  %masked = and i32 %r, 7
  call i32 @closed_sink(i32 %masked)
  ret void
}
define void @closed_wrapper() {
  call void @closed_source()
  ret void
}
define i32 @constant_sink(i32 %x) {
  %sum = add i32 %x, 8
  ret i32 %sum
}
define void @constant_caller(i32 %unrelated) {
  call i32 @constant_sink(i32 1)
  ret void
}
define i32 @prior_flag(i32 %x) {
  %first = add nuw i32 %x, 8
  %sum = sub i32 %first, 8
  ret i32 %sum
}
define i32 @nsw_guard_wrap(i32 %x) {
entry:
  %a = add nsw i32 %x, 1
  %wrapped = icmp slt i32 %a, %x
  br i1 %wrapped, label %risk, label %exit
risk:
  %sum = add nsw i32 %x, 1
  ret i32 %sum
exit:
  ret i32 0
}
define i32 @two_calls(i32 %x) {
  %ok = icmp ule i32 %x, 1000
  br i1 %ok, label %safe, label %unsafe
safe:
  %a = call i32 @raw(i32 %x)
  ret i32 %a
unsafe:
  %b = call i32 @raw(i32 %x)
  ret i32 %b
}
define i8 @poison_before_mul(i8 %x) {
entry:
  %is_limit = icmp eq i8 %x, 127
  br i1 %is_limit, label %risk, label %exit
risk:
  %first = add nsw i8 %x, 1
  %later = mul i8 %first, 2
  ret i8 %later
exit:
  ret i8 0
}
define i8 @poison_divisor_risk(i8 %x) {
entry:
  %is_limit = icmp eq i8 %x, 127
  br i1 %is_limit, label %risk, label %exit
risk:
  %first = add nsw i8 %x, 1
  %later = udiv i8 4, %first
  ret i8 %later
exit:
  ret i8 0
}
declare void @external_pointer_write(ptr)
define i32 @escape_select(ptr %p, i1 %choice) {
entry:
  %chosen = select i1 %choice, ptr %p, ptr null
  call void @external_pointer_write(ptr %chosen)
  %v = load i32, ptr %p
  %sum = add i32 %v, 1
  ret i32 %sum
}
define i32 @escape_gep(ptr %p) {
entry:
  %same = getelementptr i32, ptr %p, i64 0
  call void @external_pointer_write(ptr %same)
  %v = load i32, ptr %p
  %sum = add i32 %v, 1
  ret i32 %sum
}
define i32 @escape_phi(ptr %p, i1 %choice) {
entry:
  br i1 %choice, label %yes, label %no
yes:
  br label %merge
no:
  br label %merge
merge:
  %chosen = phi ptr [ %p, %yes ], [ null, %no ]
  call void @external_pointer_write(ptr %chosen)
  %v = load i32, ptr %p
  %sum = add i32 %v, 1
  ret i32 %sum
}
