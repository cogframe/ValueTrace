%Record = type { i32 }
@record = external global %Record
define i32 @GlobalA(i32 %x) { ret i32 %x }
