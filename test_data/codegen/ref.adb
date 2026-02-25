with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   procedure foo(x: in out Integer) is
      x2: Integer;
   begin
      Put_Line(Integer'Image(x));
      x := 2;
      Put_Line(Integer'Image(x));
   end foo;

   procedure foo2(x: in out Integer) is 
      v: Integer := x;
   begin
      Put_Line(Integer'Image(v));
   end foo2;

   v: Integer := 3;
begin
   Put_Line(Integer'Image(v));
   foo(v);
   Put_Line(Integer'Image(v));

   foo(11234);
   foo2(1234234);

end TestLoops;
