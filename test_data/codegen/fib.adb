with Ada.Text_IO;         use Ada.Text_IO;

procedure TestLoops is
    procedure fib(a: Integer; b: Integer; cur: Integer; max: Integer) is
    begin
        if cur < max then
            Put_Line(Integer'Image(b));
            fib(b, a + b, cur + 1, max);
        end if;
    end fib;

    max: Integer := 0;
begin
    Get(max);
    
    if max > 0 then
        Put_Line("0");  
        fib(0, 1, 1, max);
    end if;
end TestLoops;