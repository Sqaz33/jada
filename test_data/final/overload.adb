with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

   procedure foo(x: Integer) is
   begin
      Put_Line("foo(x: Integer)");
   end foo;

   procedure foo(X: Float) is
   begin
      Put_Line("foo(x: Float)");
   end foo;

   procedure foo(X: String) is
   begin
      Put_Line("foo(x: String)");
   end foo;

begin

   foo(1.0);
   foo("hi");
   foo(1);
   
end TestLoops;
