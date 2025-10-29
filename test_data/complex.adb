with Ada.Text_IO; use Ada.Text_IO;

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
   arr : array(1..10) of Integer := (1, 2, 3, 4);

   arr2: array(1..3) of Pack.Base := ((X => 1), (X => 2), (X => 3));

   procedure Proc is 
   begin 
      Put("Hi from ada");
      New_Line;
   end Proc;

   type IntArray is array(1 .. 100) of Integer;
   procedure Sort(arr: in out IntArray; len: Integer) is 
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
end TestLoops;
