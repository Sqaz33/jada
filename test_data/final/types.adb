with Ada.Text_IO; use Ada.Text_IO;

procedure TestLoops is
   ai: Integer := 4;
   bi: Integer := 2;

   ab: Boolean := True;
   bb: Boolean := False;
   
   ac: Character := 'h';
   bc: Character := 'i';

   af: Float := 4.0;
   bf: Float := 2.0;

   as: String := "Hello";
   bs: String := "World!";

begin
   Put_Line("Integer");
   Put_Line(Integer'Image(ai + bi));
   Put_Line(Integer'Image(ai - bi));
   Put_Line(Integer'Image(ai * bi));
   Put_Line(Integer'Image(ai / bi));

   Put_Line("Boolean");
   Put_Line(Boolean'Image((ab and bb)));
   Put_Line(Boolean'Image((ab or bb)));
   Put_Line(Boolean'Image((ab xor bb)));

   Put_Line("Character");
   Put_Line(Character'Image(ac) & Character'Image(bc));

   Put_Line("Float");
   Put_Line(Float'Image(af + bf));
   Put_Line(Float'Image(af - bf));
   Put_Line(Float'Image(af * bf));
   Put_Line(Float'Image(af / bf));

   Put_Line("String");
   Put_Line(as & " " & bs);

end TestLoops;
