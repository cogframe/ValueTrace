define i8 @add_plain(i8 %a, i8 %b) {
  %r = add i8 %a, %b
  ret i8 %r
}
define i8 @add_nsw(i8 %a, i8 %b) {
  %r = add nsw i8 %a, %b
  ret i8 %r
}
define i8 @add_nuw(i8 %a, i8 %b) {
  %r = add nuw i8 %a, %b
  ret i8 %r
}
define i64 @add64_nsw(i64 %a, i64 %b) {
  %r = add nsw i64 %a, %b
  ret i64 %r
}
define i8 @sub_plain(i8 %a, i8 %b) {
  %r = sub i8 %a, %b
  ret i8 %r
}
define i8 @sub_nsw(i8 %a, i8 %b) {
  %r = sub nsw i8 %a, %b
  ret i8 %r
}
define i8 @sub_nuw(i8 %a, i8 %b) {
  %r = sub nuw i8 %a, %b
  ret i8 %r
}
define i8 @mul_plain(i8 %a, i8 %b) {
  %r = mul i8 %a, %b
  ret i8 %r
}
define i8 @mul_nsw(i8 %a, i8 %b) {
  %r = mul nsw i8 %a, %b
  ret i8 %r
}
define i8 @mul_nuw(i8 %a, i8 %b) {
  %r = mul nuw i8 %a, %b
  ret i8 %r
}
define i8 @shl_plain(i8 %a, i8 %b) {
  %r = shl i8 %a, %b
  ret i8 %r
}
define i8 @shl_nsw(i8 %a, i8 %b) {
  %r = shl nsw i8 %a, %b
  ret i8 %r
}
define i8 @shl_nuw(i8 %a, i8 %b) {
  %r = shl nuw i8 %a, %b
  ret i8 %r
}
define i8 @lshr_plain(i8 %a, i8 %b) {
  %r = lshr i8 %a, %b
  ret i8 %r
}
define i8 @ashr_plain(i8 %a, i8 %b) {
  %r = ashr i8 %a, %b
  ret i8 %r
}
define i8 @lshr_exact(i8 %a, i8 %b) {
  %r = lshr exact i8 %a, %b
  ret i8 %r
}
define i8 @ashr_exact(i8 %a, i8 %b) {
  %r = ashr exact i8 %a, %b
  ret i8 %r
}
define i8 @udiv_plain(i8 %a, i8 %b) {
  %r = udiv i8 %a, %b
  ret i8 %r
}
define i8 @sdiv_plain(i8 %a, i8 %b) {
  %r = sdiv i8 %a, %b
  ret i8 %r
}
define i8 @urem_plain(i8 %a, i8 %b) {
  %r = urem i8 %a, %b
  ret i8 %r
}
define i8 @srem_plain(i8 %a, i8 %b) {
  %r = srem i8 %a, %b
  ret i8 %r
}
define i8 @udiv_exact(i8 %a, i8 %b) {
  %r = udiv exact i8 %a, %b
  ret i8 %r
}
define i8 @sdiv_exact(i8 %a, i8 %b) {
  %r = sdiv exact i8 %a, %b
  ret i8 %r
}
define i8 @trunc_plain(i16 %a) {
  %r = trunc i16 %a to i8
  ret i8 %r
}
define i8 @trunc_nsw(i16 %a) {
  %r = trunc nsw i16 %a to i8
  ret i8 %r
}
define i8 @trunc_nuw(i16 %a) {
  %r = trunc nuw i16 %a to i8
  ret i8 %r
}
define i16 @zext_plain(i8 %a) {
  %r = zext i8 %a to i16
  ret i16 %r
}
define i16 @zext_nneg(i8 %a) {
  %r = zext nneg i8 %a to i16
  ret i16 %r
}
define i16 @sext_plain(i8 %a) {
  %r = sext i8 %a to i16
  ret i16 %r
}
define i8 @or_disjoint(i8 %a, i8 %b) {
  %r = or disjoint i8 %a, %b
  ret i8 %r
}
define i1 @icmp_samesign(i8 %a, i8 %b) {
  %r = icmp samesign ult i8 %a, %b
  ret i1 %r
}
