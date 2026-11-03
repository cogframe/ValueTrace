define i64 @meaning(i64 %x, i64 %y, i32 %a, i32 %b) {
entry:
  %signed_compare = icmp slt i64 %x, %y
  %unsigned_compare = icmp ult i64 %x, %y
  %signed_compare32 = icmp slt i32 %a, %b
  %unsigned_compare32 = icmp ult i32 %a, %b
  %signed_div = sdiv exact i64 %x, %y
  %unsigned_div = udiv i32 %a, %b
  %signed_extend = sext i32 %a to i64
  %unsigned_extend = zext i32 %a to i64
  %nonnegative_extend = zext nneg i32 %a to i64
  %same_sign_compare = icmp samesign ult i64 %x, %y
  %disjoint_or = or disjoint i64 %x, %y
  %signed_flag = add nsw i64 %x, %y
  %unsigned_flag = add nuw i32 %a, %b
  %both_flags = mul nsw nuw i64 %x, %y
  %plain_add = add i64 %x, %y
  %narrow = trunc i64 %x to i32
  %narrow_flags = trunc nsw nuw i64 %x to i32
  %logical_shift = lshr exact i64 %x, 1
  %arithmetic_shift = ashr i64 %x, 1
  %difference = sub i64 %x, %y
  ret i64 %difference
}

define i32 @risk_cases(i32 %a, i32 %b) {
entry:
  %add_one = add i32 %a, 1
  %add_zero = add i32 %a, 0
  %subtract = sub i32 %a, %b
  %subtract_same = sub i32 %a, %a
  %multiply_two = mul i32 %a, 2
  %multiply_one = mul i32 %a, 1
  %divide = udiv i32 %a, %b
  %divide_const = udiv i32 %a, 2
  %signed_divide = sdiv i32 %a, %b
  %signed_divide_const = sdiv i32 %a, 2
  %unsigned_constant_wrap = add i32 -1, 1
  %signed_constant_overflow = add i32 2147483647, 1
  ret i32 %subtract
}
