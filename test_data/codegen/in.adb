with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   x: Integer := 1;
   s: String(1..2) := "01";
   f: Float := 1.0;
   b: Boolean := True;
   c: Character := 'c';
begin
   Get(x);
   Put_Line(Integer'Image(x));

   Get(s);
   Put_Line(s);

   Get(f);
   Put_Line(Float'Image(f));

   Get(b);
   Put_Line(Boolean'Image(b));

   Get(c);
   Put_Line(Character'Image(c));

end TestLoops;

