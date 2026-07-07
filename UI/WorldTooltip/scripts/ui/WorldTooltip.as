package ui {
    import flash.display.MovieClip;
    import flash.text.TextField;
    import data.Localization;

    public class WorldTooltip extends MovieClip {
        public var tooltipText:TextField;

        public function WorldTooltip() {
            super();
            stop();
            addFrameScript(9, onAnimationEnd);
            this.visible = false;
        }

        private function onAnimationEnd():void {
            stop();
        }

        public function show(textID:uint):void {
            tooltipText.text = Localization.resolve(textID);
            this.visible = true;
            gotoAndPlay(0);
        }

        public function hide():void {
            stop();
            this.visible = false;
        }
    }
}