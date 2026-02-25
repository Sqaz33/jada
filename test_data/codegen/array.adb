with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1..4) of Integer;

   a: arr := (1, 2, 3, 4);

   a2: array(1..2, 1..2, 1..2) of Integer;

   a22: array(1..2, 1..2, 1..2) of Integer;

   procedure printFirst(a: arr) is
   begin
      Put_Line(Integer'Image(a(1)));
   end printFirst;

   a3: arr; 

   type rec1 is record
      X: Integer := 1;
      Y: Integer := 2;
   end record;

   points: array(1..2, 1..2) of rec1;
   points2: array(1..2, 1..2) of rec1;

begin   
   --  a2(1, 1, 1) := 1;
   --  a22 := a2;
   --  Put_Line(Integer'Image(a22(1, 1, 1)));


   points(1, 1).X := 1234;

   Put_Line(Integer'Image(points(1, 1).X));

   points2 := points;
   Put_Line(Integer'Image(points2(1, 1).X));

   points2(1, 1).X := 5555;
   Put_Line(Integer'Image(points(1, 1).X));
   Put_Line(Integer'Image(points2(1, 1).X));

   --  Put_Line(Integer'Image(a3(1)));
   --  a3 := a;
   --  Put_Line(Integer'Image(a3(1)));
   --  a(1) := 2;
   
   --  for i in 1..4 loop
   --     Put_Line(Integer'Image(a(i)));
   --  end loop;

   --  a2(1, 1, 1) := 5;
   --  Put_Line(Integer'Image(a2(1, 1, 1)));

   --  a(1) := 12341243;
   --  printFirst(a);

end TestLoops;
