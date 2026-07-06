package ui.components {
	import scaleform.clik.controls.ListItemRenderer;
	import flash.text.TextField;
	import flash.display.MovieClip;
	import enums.RowType;

	public class ItemRowRenderer extends ListItemRenderer
	{
		public var name_txt:TextField;
		public var weight_txt:TextField;
		public var icon_mc:MovieClip;

		public function ItemRowRenderer()
		{
			super();
			this.stop();
		}

		override public function setData(data:Object):void
		{
			super.setData(data);

			if (data == null) return;

			this.visible = true;

			switch (data.type) {
				case RowType.HEADER:
					this.gotoAndStop("state_header");
					this.name_txt.text = data.name;
					break;
				case RowType.ITEM:
					this.gotoAndStop("state_item");
					this.name_txt.text = data.name;
					this.weight_txt.text = String(data.weight);
					break;
				default:
					this.visible = false;
					break;
			}
		}
	}
}