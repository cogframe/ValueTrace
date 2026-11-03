define i16 @narrow32(i32 %x) {
  %r = trunc i32 %x to i16
  ret i16 %r
}
define i32 @narrow64(i64 %x) {
  %r = trunc nsw nuw i64 %x to i32
  ret i32 %r
}
define i1 @narrow1(i32 %x) {
  %r = trunc i32 %x to i1
  ret i1 %r
}
define i16 @unsigned_guard(i32 %x) {
  %ok = icmp ule i32 %x, 65535
  br i1 %ok, label %safe, label %exit
safe:
  %r = trunc i32 %x to i16
  ret i16 %r
exit:
  ret i16 0
}
define i16 @signed_guard(i32 %x) {
  %lo = icmp sge i32 %x, -32768
  %hi = icmp sle i32 %x, 32767
  %ok = and i1 %lo, %hi
  br i1 %ok, label %safe, label %exit
safe:
  %r = trunc i32 %x to i16
  ret i16 %r
exit:
  ret i16 0
}
define i16 @negative_one(i32 %unused) {
  %r = trunc i32 -1 to i16
  ret i16 %r
}
define i16 @positive_max(i32 %unused) {
  %r = trunc i32 65535 to i16
  ret i16 %r
}
define i16 @safe_constant(i32 %unused) {
  %r = trunc i32 123 to i16
  ret i16 %r
}
define i16 @late_narrow_guard(i32 %x) {
  %r = trunc i32 %x to i16
  %ok = icmp ule i32 %x, 32767
  %selected = select i1 %ok, i16 %r, i16 0
  ret i16 %selected
}
define i16 @unknown_narrow(i32 %x, ptr %p) {
  %v = load i32, ptr %p
  %sum = xor i32 %x, %v
  %r = trunc i32 %sum to i16
  ret i16 %r
}
define i32 @left32(i32 %x, i32 %n) {
  %r = shl nsw nuw i32 %x, %n
  ret i32 %r
}
define i64 @left64(i64 %x, i64 %n) {
  %r = shl nsw nuw i64 %x, %n
  ret i64 %r
}
define i32 @logical32(i32 %x, i32 %n) {
  %r = lshr exact i32 %x, %n
  ret i32 %r
}
define i64 @arithmetic64(i64 %x, i64 %n) {
  %r = ashr exact i64 %x, %n
  ret i64 %r
}
define i32 @amount_guard(i32 %x, i32 %n) {
  %ok = icmp ult i32 %n, 32
  br i1 %ok, label %safe, label %exit
safe:
  %r = shl i32 %x, %n
  ret i32 %r
exit:
  ret i32 0
}
define i64 @fully_guarded(i64 %x, i64 %n) {
  %small = icmp ule i64 %x, 1000
  %amount = icmp ult i64 %n, 8
  %ok = and i1 %small, %amount
  br i1 %ok, label %safe, label %exit
safe:
  %r = shl nsw nuw i64 %x, %n
  ret i64 %r
exit:
  ret i64 0
}
define i32 @negative_shift(i32 %n) {
  %negative = icmp slt i32 %n, 0
  br i1 %negative, label %bad, label %exit
bad:
  %r = lshr i32 1, %n
  ret i32 %r
exit:
  ret i32 0
}
define i32 @only_upper_guard(i32 %n) {
  %upper = icmp slt i32 %n, 32
  br i1 %upper, label %bad, label %exit
bad:
  %r = lshr i32 1, %n
  ret i32 %r
exit:
  ret i32 0
}
define i32 @unknown_shift(i32 %x, ptr %p) {
  %n = load i32, ptr %p
  %r = lshr i32 %x, %n
  ret i32 %r
}
define i32 @unknown_left(i32 %n, ptr %p) {
  %x = load i32, ptr %p
  %r = lshr i32 %x, %n
  ret i32 %r
}
define i32 @constant_shift(i32 %unused) {
  %safe = shl i32 1, 1
  %wrap = shl i32 -1, 1
  %zero = shl i32 0, 1
  %identity = shl i32 3, 0
  %bad = ashr i32 0, 32
  ret i32 %bad
}
define i32 @prior_poison(i32 %x) {
  %poison = shl i32 %x, 32
  %r = trunc i32 %poison to i16
  %extended = zext i16 %r to i32
  ret i32 %extended
}
define i32 @late_shift_guard(i32 %x, i32 %n) {
  %r = shl i32 %x, %n
  %ok = icmp ult i32 %n, 32
  %selected = select i1 %ok, i32 %r, i32 0
  ret i32 %selected
}
