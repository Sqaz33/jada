with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arr is array(1..10) of Integer;

   a: arr := (1, 5, 7, 19, 10, 1, 2, 3, 4 ,5);

   procedure Bubble_Sort(a: in out arr; N: Integer) is
      procedure Print_Array(a : arr; N: Integer) is
      begin
         for I in 1..N loop
            Put_Line(Integer'Image(a(I)) & " - ");
         end loop;
      end Print_Array;

      Temp : Integer := 0;

   begin
      Put_Line("in arr:");
      Print_Array(A, N);

      for I in 1 .. N - 1 loop
         for J in 1 .. N - I loop
            if A(J) > A(J + 1) then
               Temp := A(J);
               A(J) := A(J + 1);
               A(J + 1) := Temp;
            end if;
         end loop;
      end loop;

      Put_Line("out arr:");
      Print_Array(A, N);

   end Bubble_Sort;

begin   
   Bubble_Sort(a, 10);

end TestLoops;
