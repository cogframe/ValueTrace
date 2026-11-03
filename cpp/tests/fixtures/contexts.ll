define i32 @identity(i32 %x) {
entry:
  ret i32 %x
}

define i32 @pair(i32 %a, i32 %b) {
entry:
  %first = call i32 @identity(i32 %a)
  %second = call i32 @identity(i32 %b)
  %sum = add i32 %first, %second
  ret i32 %sum
}

define i32 @memory(ptr %p, i32 %count) {
entry:
  %loaded = load i32, ptr %p
  %sum = add i32 %loaded, %count
  ret i32 %sum
}

define i32 @indirect(i32 %count, ptr %fn) {
entry:
  %answer = call i32 %fn(i32 %count)
  ret i32 %answer
}

define i32 @recursive(i32 %count) {
entry:
  %next = sub i32 %count, 1
  %answer = call i32 @recursive(i32 %next)
  ret i32 %answer
}

declare i32 @external(i32)
define i32 @external_user(i32 %x) {
entry:
  %answer = call i32 @external(i32 %x)
  ret i32 %answer
}
