package pbfy0
{
	import BML.ModSprite;
	import _bh_.Main;
	
	/**
	 * ...
	 * @author Paul F-Y
	 */
	public class MultiKeyboard extends ModSprite
	{
		
		override protected function CInit() : void {
			Main._bh_MULTIKEYBOARD_ENABLED = true;
		}
		
		override protected function Unload() : void {
			Main._bh_MULTIKEYBOARD_ENABLED = false;
		}
		
	}
	
}