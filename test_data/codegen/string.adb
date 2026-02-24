with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   str: String(1..2) := "12";

begin
   --  Put_Line("Hi from ada");
   --  Put_Line(str);

   Put_Line(str);
   str(1) := '0';
   Put_Line(str);

   str(2) := '3';
   Put_Line(str);
end TestLoops;
