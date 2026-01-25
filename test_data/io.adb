with Ada.Text_IO;
use Ada.Text_IO;

procedure Main is
   X : Integer;
begin
   Put("Введите целое число: ");
   Get(X);
   Put_Line("Вы ввели: " & Integer'Image(X));
end Main;
