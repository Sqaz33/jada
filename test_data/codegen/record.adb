with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type rec1 is record
      X: Integer := 1;
      Y: Integer := 2;
   end record;

   v: rec1;
begin
   v.X := 5;
   Put_Line(Integer'Image(v.X));

end TestLoops;
