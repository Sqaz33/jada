with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

   package pack1 is
      procedure foo(x: Integer);
      function foo(X: Integer) return Integer;
      x: Integer := 1;
   end pack1;

   package body pack1 is
      procedure foo(x: Integer) is
      begin
         Put_Line(Integer'Image(foo(x)));
      end foo;

      function foo(X: Integer) return Integer is
      begin
         return x + 10;
      end foo;

      l: Integer := foo(1);
   end pack1;

   x: Integer;
begin

   pack1.foo(11234);
   --  Put_Line(Integer'Image(pack1.x));
   x := 1;

end TestLoops;
