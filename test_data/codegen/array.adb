with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1..4) of Integer;

   a1: arr := (1, 2, 3, 4);
   a2: arr := (5, 6, 7, 8);

   a12: array(1..2, 1..2, 1..2) of Integer;
   a22: array(1..2, 1..2, 1..2) of Integer;

   procedure printFirst(a: arr) is
   begin
      Put_Line(Integer'Image(a(1)));
   end printFirst;

   procedure setFirst(a: in out arr) is
   begin
      a(1) := 666666;
   end setFirst;

   procedure printArr(a: arr) is
   begin
      for i in 1..4 loop
         Put_Line(Integer'Image(a(i)));
      end loop;
   end printArr;

   a3: arr; 

   type rec1 is record
      X: Integer := 1;
      Y: Integer := 2;
   end record;

   points: array(1..2, 1..2) of rec1;
   points2: array(1..2, 1..2) of rec1;

begin   
   printFirst(a1);
   printFirst(a2);

   Put_Line("pre a1:");
   printArr(a1);
   Put_Line("pre a2:");
   printArr(a2);
   a1 := a2;
   Put_Line("post a1:");
   printArr(a1);
   Put_Line("post a2:");
   printArr(a2);

   a12(2, 2, 2) := 55;
   a22(2, 2, 2) := 33;
   Put_Line("prea a12: ");
   Put_Line (Integer'Image(a12(2, 2, 2)));
   Put_Line("prea a22: ");
   Put_Line (Integer'Image(a22(2, 2, 2)));
   a12 := a22;
   Put_Line("post a12: ");
   Put_Line (Integer'Image(a12(2, 2, 2)));
   Put_Line("post a22: ");
   Put_Line (Integer'Image(a22(2, 2, 2)));


   points(2, 2).X := 44;
   points2(2, 2).X := 66;
   Put_Line("prea points: ");
   Put_Line (Integer'Image(points(2, 2).X));
   Put_Line("prea points2: ");
   Put_Line (Integer'Image(points2(2, 2).X));
   points := points2;
   Put_Line("post points: ");
   Put_Line (Integer'Image(points(2, 2).X));
   Put_Line("post points2: ");
   Put_Line (Integer'Image(points2(2, 2).X));

   Put_Line("prea first a1: ");
   printFirst(a1);
   setFirst(a1);
   Put_Line("post first a1: ");
   printFirst(a1);

end TestLoops;
