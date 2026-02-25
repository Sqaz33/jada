with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

   procedure foo(x: Integer) is
   begin
      Put_Line(Integer'Image(x));
   end foo;

   function foo(X: Integer) return Integer is
   begin
      return x;
   end foo;

   x: Integer := foo(2);

begin
   foo(1);
   Put_Line(Integer'Image(x));
end TestLoops;
