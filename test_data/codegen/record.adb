with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type rec1 is record
      X: Integer := 1;
      Y: Integer := 2;
   end record;

   v: rec1;
 
   procedure proc(r: out rec1) is
   begin
      r.Y := 10;
   end proc;
   
   v2: rec1;

begin
   v.X := 5;
   Put_Line(Integer'Image(v.X));

   proc(v);
   Put_Line(Integer'Image(v.Y));

   Put_Line("====================");
   Put_Line(Integer'Image(v2.X));
   Put_Line(Integer'Image(v2.Y));
   v2 := v;
   Put_Line(Integer'Image(v2.X));
   Put_Line(Integer'Image(v2.Y));

end TestLoops;
