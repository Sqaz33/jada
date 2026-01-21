with Ada.Text_IO; use Ada.Text_IO;
--  with complex;

--  package Pack is 

--     type Base is record 
--        X: Integer := 1;
--     end record;

--     function print(self: Base) return Integer is
--     begin
--        Put_Line(Integer'Image(self.X));
--        return 1;
--     end print;

--  end Pack;

procedure TestLoops is
   i : Integer;
   sum : Integer := 0;
   L: String(1..2) := "1234";
   arr : array(1..10) of Integer := (1, 2, 3, 4);

   arr2: array(1..3) of Integer;

   type IntArray is array(1 .. 100) of Integer;

   function Func(X: Integer; Y:  Integer) return Integer is
   begin
      return x + Y;
   end Func;  

   procedure Proc (X: IntArray) is 
   begin 
      Put("Hi from ada");
      New_Line;
   end Proc;

   procedure Proc (X: Integer) is 
   begin 
      Put("Hi from ada");
      New_Line;
   end Proc;

   procedure Sort(arr: IntArray; len: Integer) is 
      min: Integer;
      buf: Integer;
      begin
         for i in 1..len loop 
            min := i;
            for j in i..len loop
               if arr(j) < min then
                  min := j;
               end if;
            end loop;
            buf := arr(i);
            arr(i) := arr(min);
            arr(min) := buf;
         end loop;
   end Sort;
   
begin
   if 1 > 0 then
      if 2 > 0 then 
         x := x + 1;
      else 
         x := x + 1;
      end if;
   end if;
   -- Цикл for
    for i in 1..10 loop
        arr(i) := i * 2;
        sum := sum + arr(i);
    end loop;

   if I = 0 then
      I := 1;
   elsif I = 2 then
      I := 3;
   else 
      I := 4;
   end if;
    
   -- Цикл while
    i := 1;
    while i <= 10 loop
        i := i + 1;
    end loop;
    
    -- Вложенные циклы
    for i in 1..5 loop
        for j in 1..5 loop
            if i = j then
                arr(i) := arr(i) + j;
            end if;
        end loop;
    end loop;
   I := 1 + 2 + 3 + 4 + 5;

end TestLoops;
