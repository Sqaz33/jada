with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   x: Integer := 100;
begin
   if X < 10 then
      Put_Line("< 10");
   elsif X > 20 then
      Put_Line("> 20");
   else
      Put_Line("10 <= x <= 20");
   end if;

   if X < 10 then
      Put_Line("< 10 2");
   elsif X > 20 then
      Put_Line("> 20 2");
   else
      Put_Line("10 <= x <= 20 2");
   end if;

   if (X > 10 and x /= 100) then 
      Put_Line("X > 10 and x /= 101");
   else
      Put_Line(">~<");
   end if;

   if (X > 10 and x /= 101) then 
      Put_Line("X > 10 and x /= 101");
   end if;
end TestLoops;
