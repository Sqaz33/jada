with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   i: Integer := 1;
   x: Integer := 1;
begin
   for i in 1..67 loop
      Put_Line (Integer'Image(i));
   end loop;

   for i in 100..200 loop
      Put_Line (Integer'Image(i));
   end loop;

   while (i < 10 and i /= 7) loop
      Put_Line(Integer'Image(i));
      i := i + 1;
   end loop;

   Put_Line(Integer'Image(i + 100000));

   Get(x);
   for i in 100..x loop
      Put_Line (Integer'Image(i));
   end loop;

end TestLoops;
