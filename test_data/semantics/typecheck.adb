procedure Main is
   type arr is array(1..3) of Integer;

   a: arr := (1, 2, 3);

   str: String(1..2) := "12";

   function foo(x: Integer) return Integer is
   begin
      return x;
   end foo;

   procedure foo(x: in out String) is
   begin
      x(1) := 'c';
      return;
   end foo;

   procedure foo1(x: in out String(1..2)) is
   begin
      x(1) := 'c';
      return;
   end foo1;

   function foo(x: Boolean) return String(1..3) is
   begin
      return "123";
   end foo;

   var1: Integer;
   var2: Integer;

   str2: String(1..4) := "he" & "t" & "t";

   --  bx : Boolean := (true and true); -- false

   arrarr1: array(1..2, 1..2) of Integer;

begin

   a := (4, 5, 6);

   str := "13";

   var1 := 1 + var2 + var1 + 1 + 2 + foo(foo(var1));

   foo(str);
   foo1(str);

   str2 := "he" & "tt";

   --  var1 := 1 + 1.0; -- err
   var1 := 1 + 1 + 1; -- bug 

   --  str := "1" & foo(1); -- err

   str2 := "1" & foo(True);

   --  str2(1, 1) := 'c'; -- err
   str2(1) := 'c';

   --  arrarr1(1) := 1; -- err
   arrarr1(1, 1) := 2;
   
   for i in 1..foo(10) loop
      var1 := i;
   end loop;   
end Main;
