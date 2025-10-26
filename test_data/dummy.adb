with Ada.Text_IO; use Ada.Text_IO;

package Pack is 

type Base is record 
   X: Integer := 1;
end record;

function print(self: Base) return Integer is
begin
   Put_Line(Integer'Image(self.X));
   return 1;
end print;

end Pack;


-- Тестирование циклов
procedure TestLoops is
    i : Integer;
    sum : Integer := 0;
    arr : array(1..10) of Integer := (1, 2, 3, 4);
begin
   --   -- Цикл for
    for i in 1..10 loop
        arr(i) := i * 2;
        sum := sum + arr(i);
    end loop;
    
   --   -- Цикл while
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