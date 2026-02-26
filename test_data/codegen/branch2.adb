with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   flag: array(1..2) of Boolean := (True, True);
   i : Integer := 3;
begin
   while ((i = 3 and true) or flag(i)) loop
      Put_Line ("Ok");
      i := i - 1;
   end loop;

   if (10 > 1 or flag(3)) then
      Put_Line ("Ok");
   end if;

   --  i := 3;
   --  while (i = 3 or flag(i)) loop
   --     Put_Line ("Ok");
   --     i := i - 1;
   --  end loop;
end TestLoops;
