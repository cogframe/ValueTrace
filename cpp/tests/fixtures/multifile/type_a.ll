declare i32 @conflict(i32)
define i32 @typeCaller(i32 %x) {
  %r = call i32 @conflict(i32 %x)
  ret i32 %r
}
