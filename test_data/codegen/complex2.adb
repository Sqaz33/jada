with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   i : Integer := 1;
   x: Integer := 1;
   sum : Integer := 0;
   L: String(1..4) := "1234";
   arr : array(1..4) of Integer := (1, 2, 3, 4);

   arr2: array(1..3) of Integer;

   type IntArray is array(1 .. 10) of Integer;

   function Func(X: Integer; Y:  Integer) return Integer is
   begin
      return x + Y;
   end Func;  

   procedure Proc (X: IntArray) is 
   begin 
      Put_Line("Hi from ada");
   end Proc;

   procedure Proc (X: Integer) is 
   begin 
      Put_Line("Hi from ada");
   end Proc;

   type Ty12 is record 
      X: Integer;
   end record;

   function ty12f return Ty12 is
      obj: Ty12;
   begin
      return obj;
   end ty12f;

   objty12: Ty12;

   arrty12: array(1..10, 1..10, 1..10, 1..10) of Ty12;

   procedure Sort(arr: in out IntArray; len: Integer) is 
      min: Integer := 0;
      buf: Integer := 0;
      begin
         for i in 1..len-1 loop 
            min := i;
            for j in i..len loop
               if arr(j) < arr(min) then
                  min := j;
               end if;
            end loop;
            buf := arr(i);
            arr(i) := arr(min);
            arr(min) := buf;
         end loop;
   end Sort;

   procedure Insertion(arr: in out IntArray; len: Integer) is
      j: Integer;
      buf: Integer;
   begin
      for i in 1..len-1 loop
         j := i+1;
         --  Put_Line(Integer'Image(i));
         while (j >= 2 and arr(j) < arr(j-1)) loop
            buf := arr(j);
            arr(j) := arr(j-1);
            arr(j-1) := buf;
            j := j - 1;
            --  Put_Line(Integer'Image(j));
            --  Put_Line(Integer'Image(i));
         end loop; 
      end loop;
   end Insertion;

   arr1234: IntArray :=  (5, 1, 1, 7, 9, 3, 4, 0, 10, 16);
   --  (10, 9, 8, 6, 5, 4, 3, 2, 1, 0);
begin
   if 1 > 0 then
      if 2 > 0 then 
         x := x + 1;
      else 
         x := x + 1;
      end if;
   end if;
   -- Цикл for
   --   for i in 1..4 loop
   --       arr(i) := i * 2;
   --       sum := sum + arr(i);
   --   end loop;

   if I = 0 then
      I := 1;
   elsif I = 2 then
      I := 3;
   else 
      I := 4;
   end if;
    
   --  Цикл while
    i := 1;
    while i <= 10 loop
        i := i + 1;
    end loop;
    
    -- Вложенные циклы
   --   for i in 1..4 loop
   --       for j in 1..4 loop
   --           if i = j then
   --               arr(i) := arr(i) + j;
   --              --   Put_Line(Integer'Image(i * 8 / 2 + j * 10));
   --           end if;
   --       end loop;
   --   end loop;

   I := 1 + 2 + 3 + 4 + 5;

   --  ty12f.x := 2; -- err pure rval assign
   objty12.x := 2;

   Sort(arr1234, 10);
   --  Insertion(arr1234, 10);

   for i in 1..10 loop
      Put_Line(Integer'Image(arr1234(i)));
   end loop;
   
end TestLoops;
