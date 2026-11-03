declare void @guard_sink(i32)
define void @guard(i32 %x) {
entry:
  %ok = icmp ult i32 %x, 10
  br i1 %ok, label %call, label %exit
call:
  %y = add i32 %x, 1
  call void @guard_sink(i32 %y)
  br label %exit
exit:
  ret void
}

declare void @phi_sink(i32)
define void @phi_case(i32 %x) {
entry:
  %test = icmp eq i32 %x, 1
  br i1 %test, label %left, label %right
left:
  br label %merge
right:
  br label %merge
merge:
  %v = phi i32 [10, %left], [20, %right]
  %allowed = icmp ne i32 %x, 1
  br i1 %allowed, label %call, label %exit
call:
  call void @phi_sink(i32 %v)
  br label %exit
exit:
  ret void
}

declare void @select_sink(i32)
define void @select_case(i32 %x) {
entry:
  %test = icmp eq i32 %x, 1
  %v = select i1 %test, i32 10, i32 20
  %allowed = icmp ne i32 %x, 1
  br i1 %allowed, label %call, label %exit
call:
  call void @select_sink(i32 %v)
  br label %exit
exit:
  ret void
}

declare void @switch_sink(i32)
define void @switch_case(i32 %x) {
entry:
  switch i32 %x, label %default [i32 1, label %one i32 2, label %two]
one:
  call void @switch_sink(i32 10)
  ret void
two:
  call void @switch_sink(i32 20)
  ret void
default:
  call void @switch_sink(i32 30)
  ret void
}

declare void @nowrap_sink(i32)
define void @nowrap() {
entry:
  %bad = add nsw i32 2147483647, 1
  call void @nowrap_sink(i32 %bad)
  ret void
}

declare void @wrap_sink(i32)
define void @wrap() {
entry:
  %bits = add i32 2147483647, 1
  call void @wrap_sink(i32 %bits)
  ret void
}

declare void @unused_poison_sink(i32)
define void @unused_poison() {
entry:
  %bad = add nsw i32 2147483647, 1
  %v = select i1 false, i32 %bad, i32 7
  call void @unused_poison_sink(i32 %v)
  ret void
}

declare void @selected_poison_sink(i32)
define void @selected_poison() {
entry:
  %bad = add nsw i32 2147483647, 1
  %v = select i1 true, i32 %bad, i32 7
  call void @selected_poison_sink(i32 %v)
  ret void
}

declare void @divide_zero_sink(i32)
define void @divide_zero() {
entry:
  %unused = udiv i32 1, 0
  call void @divide_zero_sink(i32 7)
  ret void
}

declare void @cast_sink(i64)
define void @cast_case(i32 %x) {
entry:
  %ok = icmp eq i32 %x, -1
  br i1 %ok, label %call, label %exit
call:
  %wide = sext i32 %x to i64
  call void @cast_sink(i64 %wide)
  br label %exit
exit:
  ret void
}

declare void @memory_sink(i32)
define void @memory_case(ptr %p) {
entry:
  %v = load i32, ptr %p
  call void @memory_sink(i32 %v)
  ret void
}

declare void @loop_sink(i32)
define void @loop_case(i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [0, %entry], [%next, %body]
  %go = icmp ult i32 %i, %n
  br i1 %go, label %body, label %exit
body:
  %next = add i32 %i, 1
  br label %loop
exit:
  call void @loop_sink(i32 %i)
  ret void
}

define i32 @helper(i32 %x) {
entry:
  %next = add i32 %x, 1
  ret i32 %next
}
declare void @helper_sink(i32)
define void @helper_case(i32 %x) {
entry:
  %v = call i32 @helper(i32 %x)
  call void @helper_sink(i32 %v)
  ret void
}

define void @context_sink(i32 %x) {
entry:
  ret void
}
define void @context_case(i32 %x) {
entry:
  call void @context_sink(i32 5)
  %allowed = icmp ult i32 %x, 8
  br i1 %allowed, label %call, label %exit
call:
  call void @context_sink(i32 %x)
  br label %exit
exit:
  ret void
}

declare void @poison_divisor_sink(i32)
define void @poison_divisor() {
entry:
  %unused = udiv i32 1, poison
  call void @poison_divisor_sink(i32 7)
  ret void
}

declare void @phi_poison_sink(i32)
define void @phi_poison_case(i1 %choose_bad) {
entry:
  br i1 %choose_bad, label %bad, label %safe
bad:
  %poisoned = add nsw i32 2147483647, 1
  br label %merge
safe:
  br label %merge
merge:
  %value = phi i32 [%poisoned, %bad], [7, %safe]
  call void @phi_poison_sink(i32 %value)
  ret void
}

declare void @poison_branch_sink(i32)
define void @poison_branch_case() {
entry:
  %bad = add nsw i1 1, 1
  br i1 %bad, label %call, label %exit
call:
  call void @poison_branch_sink(i32 7)
  br label %exit
exit:
  ret void
}

declare void @generated_poison_divisor_sink(i32)
define void @generated_poison_divisor_case() {
entry:
  %bad = add nsw i8 127, 1
  %unused = udiv i8 4, %bad
  call void @generated_poison_divisor_sink(i32 7)
  ret void
}

define void @does_not_return() noreturn {
entry:
  ret void
}
declare void @noreturn_sink(i32)
define void @noreturn_case() {
entry:
  call void @does_not_return()
  call void @noreturn_sink(i32 7)
  ret void
}

declare void @signed_cmp_sink(i64)
define void @signed_cmp_case(i64 %x) {
entry:
  %test = icmp slt i64 %x, 0
  %v = select i1 %test, i64 11, i64 22
  call void @signed_cmp_sink(i64 %v)
  ret void
}
declare void @unsigned_cmp_sink(i64)
define void @unsigned_cmp_case(i64 %x) {
entry:
  %test = icmp ult i64 %x, 0
  %v = select i1 %test, i64 11, i64 22
  call void @unsigned_cmp_sink(i64 %v)
  ret void
}
