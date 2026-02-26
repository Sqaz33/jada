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

   procedure foo3(x: in Integer) is
      v: Integer := x;
   begin
      v := v + 1;
   end foo3;

   l: Integer := 1114;
begin
   Put_Line(Integer'Image(v));
   foo(v);
   Put_Line(Integer'Image(v));

   foo(11234);
   foo2(1234234);

   Put_Line("==========");
   Put_Line(Integer'Image(l));
   foo3(l);
   Put_Line(Integer'Image(l));
   Put_Line("==========");

end TestLoops;
