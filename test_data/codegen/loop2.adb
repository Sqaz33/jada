with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   i: Integer := 1;
begin
   Put_Line ("pre i: " & Integer'Image(i));

   for i in 1..3 loop
      Put_Line ("outer: " & Integer'Image(i));
      for i in 1..3 loop
         Put_Line ("ineer: " &Integer'Image(i));
      end loop;
   end loop;

   Put_Line ("post i: " & Integer'Image(i));

   Get(i);

   for j in 1..i loop 
      Put_Line ("j: " & Integer'Image(j));
   end loop;

end TestLoops;
