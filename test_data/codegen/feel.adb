with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is

    type Matrix is array(1..8, 1..8) of Integer;

    procedure printMatrix(m: Matrix) is
        line: String(1..16);
    begin
        for i in 1..8 loop
            for j in 1..8 loop
                line := line & Integer'Image(m(i, j)) & " ";
            end loop;
            Put_Line(line);
            line := "";
        end loop;
    end printMatrix;

    m: Matrix;

    x: Integer := 1;

    i: Integer;

begin
    printMatrix(m);

    while x <= 4 loop

        for i in x..8-x+1 loop
            m(x, i) := 1;
        end loop; 

        for i in x..8-x+1 loop
            m(i, x) := 1;
        end loop; 

        i := 8 - x+1;
        while i >= x loop
            m(x, i) := 1;
            i := i - 1;
        end loop;

        i := 8 - x+1;
        while i >= x loop
            m(i, x) := 1;
            i := i - 1;
        end loop;

        x := x + 2;
    end loop;

    Put_Line("==================");
    printMatrix(m);

    
end TestLoops;
