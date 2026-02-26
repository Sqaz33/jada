with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   x: Integer := 100;
   flag: array(1..2) of Boolean := (True, True);

   function print return Boolean is
   begin
      Put_Line("12344234234");
      return false;
   end print;

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

   if ((X > 10 and x /= 101) and print) then 
      Put_Line("X > 10 and x /= 101");
   end if;

   if (flag(1) or print)  then
      Put_Line(Boolean'Image(flag(1)));
   end if;
end TestLoops;
