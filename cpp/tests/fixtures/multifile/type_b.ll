define i64 @conflict(i32 %x) {
  %r = zext i32 %x to i64
  ret i64 %r
}
