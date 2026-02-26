with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   --  flag: Boolean := (True or False);
   flag: Boolean := ((1 > 0 and False) or 1 < 2);
   flag2: Boolean := ((1 > 0 and False) or 1 > 2);
   flag3: Boolean := (False xor True);
   flag4: Boolean := (True xor True);
   flag5: Boolean := (not True);
begin
   --  Put_Line(Boolean'Image(1 = 1));
   Put_Line(Boolean'Image(flag));
   Put_Line(Boolean'Image(flag2));
   Put_Line(Boolean'Image(flag3));
   Put_Line(Boolean'Image(flag4));
   Put_Line(Boolean'Image(flag5));
end TestLoops;
