with Ada.Text_IO; 

package body pack is
   procedure proc(self: in out ty) is
   begin
      self.X := 1;
      Ada.Text_IO.Put_Line(Integer'Image(self.X));
   end proc;
end pack;