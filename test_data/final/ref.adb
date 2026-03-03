with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1..2) of Integer; 

   type rec is record
      x: Integer := 1;
   end record;

   procedure foo(x: in out Integer; s: in out String; a: in out arr; r: in out rec) is
   begin 
      x := 5;  
      s(1) := '5';
      a(1) := 5;
      r.x := 5;
   end foo;

   x: Integer := 1;
   s: String := "str";
   a: arr := (1, 2);
   r: rec;

begin
   Put_Line("Before");
   Put_Line("X: " & Integer'Image(x));
   Put_Line("S: " & s);
   Put_Line("A: " & Integer'Image(a(1)) & ", " & Integer'Image(a(2)));
   Put_Line ("Rec: " & Integer'Image(r.x));

   foo(x, s, a, r);

   Put_Line("After");
   Put_Line("X: " & Integer'Image(x));
   Put_Line("S: " & s);
   Put_Line("A: " & Integer'Image(a(1)) & ", " & Integer'Image(a(2)));
   Put_Line ("Rec: " & Integer'Image(r.x));

end TestLoops;
