procedure Main is
   type arr is array(1..3) of Integer;

   a: arr := (1, 2, 3);

   str: String(1..2) := "12";

   function foo(x: Integer) return Integer is
   begin
      return x;
   end foo;

   var1: Integer;
   var2: Integer;
begin
   a := (4, 5, 6);

   str := "13";

   var1 := 1 + var2 + var1 + 1 + 2 + foo(foo(var1));
end Main;
