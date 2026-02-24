with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1..4) of Integer;

   a: arr := (1, 2, 3, 4);

   a2: array(1..2, 1..2, 1..2) of Integer;

begin   
   a(1) := 2;
   
   for i in 1..4 loop
      Put_Line(Integer'Image(a(i)));
   end loop;

   a2(1, 1, 1) := 5;
   Put_Line(Integer'Image(a2(1, 1, 1)));
end TestLoops;
