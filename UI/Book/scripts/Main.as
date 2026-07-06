package
{
    import flash.display.MovieClip;
    import flash.events.Event;
    import ui.pages.BookBase;
    import data.Localization;

    public dynamic class Main extends MovieClip
    {
        public var book_mc:BookBase;

        public function Main()
        {
            super();
            this.addEventListener(Event.ADDED_TO_STAGE, onInit);
        }

        private function onInit(e:Event):void
        {
            this.removeEventListener(Event.ADDED_TO_STAGE, onInit);
            this["SetStringTable"] = Localization.setTable;
            book_mc.setMaxSpreads(1);
        }
    }
}