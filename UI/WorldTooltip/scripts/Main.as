package {
    import flash.display.MovieClip;
    import flash.events.Event;
    import data.Localization;
    import ui.WorldTooltip;

    public dynamic class Main extends MovieClip {
        public var worldTooltip_mc:WorldTooltip;

        public function Main() {
            super();
            this.addEventListener(Event.ADDED_TO_STAGE, onInit);
        }

        public function onInit(e:Event):void {
            this.removeEventListener(Event.ADDED_TO_STAGE, onInit);
            this["SetStringTable"] = Localization.setTable;
        }
    }
}