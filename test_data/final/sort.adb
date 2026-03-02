with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr1 is array(1..10) of Integer;

   a: arr1 := (0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

   procedure Print_Array(a : arr1; N: Integer) is
   begin
      for I in 1..N loop
         Put_Line(Integer'Image(a(I)));
      end loop;
   end Print_Array;

   procedure Insertion(arr: in out arr1; len: Integer) is
      j: Integer;
      buf: Integer;
   begin
      for i in 1..len-1 loop
         j := i+1;
         while (j >= 2 and arr(j) < arr(j-1)) loop
            buf := arr(j);
            arr(j) := arr(j-1);
            arr(j-1) := buf;
            j := j - 1;
         end loop; 
      end loop;
   end Insertion;

   sz: Integer := 1;

begin   
   Put_Line("array size: ");
   Get(sz);

   for i in 1..sz loop 
      Get(x);
      a(i) := x;
   end loop;

   Put_Line("==========");
   Print_Array(a, sz);
   Put_Line("==========");
   Insertion(a, sz);
   Print_Array(a, sz);

end TestLoops;
