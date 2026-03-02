with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   type arrarr is array(1..10, 1..8) of Integer; 

   a: arrarr := (
      (1, 1, 1, 1, 1, 1, 1, 1),
      (0, 0, 0, 0, 0, 0, 0, 1),
      (1, 1, 1, 1, 1, 1, 0, 1),
      (1, 0, 0, 0, 0, 1, 0, 1),
      (1, 0, 1, 1, 0, 1, 0, 1),
      (1, 0, 1, 0, 0, 1, 0, 1),
      (1, 0, 1, 0, 0, 1, 0, 1),
      (1, 0, 1, 1, 1, 1, 0, 1),
      (1, 0, 0, 0, 0, 0, 0, 1),
      (1, 1, 1, 1, 1, 1, 1, 1)
   );


   procedure fill(a: in out arrarr; old: Integer; nw: Integer; x: Integer; y: Integer) is
   begin
      if a(y, x) = old then
         a(y, x) := nw;
      end if;
      
      if ((x + 1 <= 8) and (a(y, x + 1) = old))  then -- r
         fill(a, old, nw, x + 1, y);
      end if;

      if ((y + 1 <= 10) and (a(y + 1, x) = old))  then -- d
         fill(a, old, nw, x, y + 1);
      end if;

      if ((x - 1 >= 1) and (a(y, x - 1) = old))  then -- l
         fill(a, old, nw, x - 1, y);
      end if;

      if ((y - 1 >= 1) and (a(y - 1, x) = old))  then -- u
         fill(a, old, nw, x, y - 1);
      end if;
   end fill;

   procedure printArrArr(a: arrarr) is
      line: String := "";
   begin
      for i in 1..10 loop
         for j in 1..8 loop
            line := line & Integer'Image(a(i, j)) & " ";
         end loop;
         Put_Line (line);
         line := "";
      end loop;
   end printArrArr;

begin

   printArrArr (a);
   fill(a, 1, 2, 1, 1);
   Put_Line ("==============================");
   printArrArr (a);


end TestLoops;
