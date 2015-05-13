// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.EChart = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.AMap.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	// called on startup for each object type
	typeProto.onCreate = function()
	{
    };

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
        this.axistype = ['category','value','time'];
	};
	
	var instanceProto = pluginProto.Instance.prototype;


    function loadJS(url, success, instance) {
        var domScript = document.createElement('script');
        domScript.src = url;
        success = success || function(){};
        domScript.onload = domScript.onreadystatechange = function() {
            if (!this.readyState || 'loaded' === this.readyState || 'complete' === this.readyState) {
                success(instance);
                this.onload = this.onreadystatechange = null;
                this.parentNode.removeChild(this);
            }
        }
        document.getElementsByTagName('body')[0].appendChild(domScript);
    }


    // called whenever an instance is created
	instanceProto.onCreate = function()
	{
		// Not supported in directCanvas
		if (this.runtime.isDomFree)
		{
			cr.logexport("[Orion 2] 当前平台不支持EChart插件!");
			return;
		}

        this.myChart = null;
        this.option = {
            title : {
                text: this.properties[2],
                subtext: ''
            },
            tooltip: {
                show :  this.properties[5] === 1
            },
            toolbox: {
                show : this.properties[3] === 1,
                feature : {
                    mark : {show: false},
                    dataView : {show: false, readOnly: true},
                    restore : {show: false},
                    saveAsImage : {show: true}
                }
            },
            calculable : this.properties[4] === 1
        };

		this.divElem = document.createElement("div");
        this.divElem.id = this.properties[1];
		jQuery(this.divElem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");

        this.element_hidden = false;
		
		if (this.properties[0] === 1)
		{
			jQuery(this.divElem).hide();
			this.visible = false;
			this.element_hidden = true;
		}

		this.lastLeft = 0;
		this.lastTop = 0;
		this.lastRight = 0;
		this.lastBottom = 0;
		this.lastWinWidth = 0;
		this.lastWinHeight = 0;
			
		this.updatePosition(true);

        // 增加功能
        loadJS("http://echarts.baidu.com/build/dist/echarts-all.js", function(instance){
            instance.myChart = echarts.init(instance.divElem);

        });

		this.runtime.tickMe(this);
	};
	
	instanceProto.saveToJSON = function ()
	{
		var o = {
			"disabled": !!this.divElem.disabled
		};
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.divElem.disabled = o["disabled"];
	};
	
	instanceProto.onDestroy = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		jQuery(this.divElem).remove();
        jQuery(this.scriptElem).remove();
		this.elem = null;
	};
	
	instanceProto.tick = function ()
	{
		this.updatePosition();
	};
	
	var last_canvas_offset = null;
	var last_checked_tick = -1;
	
	instanceProto.updatePosition = function (first)
	{
		if (this.runtime.isDomFree)
			return;
		
		var left = this.layer.layerToCanvas(this.x, this.y, true);
		var top = this.layer.layerToCanvas(this.x, this.y, false);
		var right = this.layer.layerToCanvas(this.x + this.width, this.y + this.height, true);
		var bottom = this.layer.layerToCanvas(this.x + this.width, this.y + this.height, false);
		
		var rightEdge = this.runtime.width / this.runtime.devicePixelRatio;
		var bottomEdge = this.runtime.height / this.runtime.devicePixelRatio;
		
		// Is entirely offscreen or invisible: hide
		if (!this.visible || !this.layer.visible || right <= 0 || bottom <= 0 || left >= rightEdge || top >= bottomEdge)
		{
			if (!this.element_hidden)
				jQuery(this.divElem).hide();
			
			this.element_hidden = true;
			return;
		}
		
		// Truncate to canvas size
		if (left < 1)
			left = 1;
		if (top < 1)
			top = 1;
		if (right >= rightEdge)
			right = rightEdge - 1;
		if (bottom >= bottomEdge)
			bottom = bottomEdge - 1;
		
		var curWinWidth = window.innerWidth;
		var curWinHeight = window.innerHeight;
			
		// Avoid redundant updates
		if (!first && this.lastLeft === left && this.lastTop === top && this.lastRight === right && this.lastBottom === bottom && this.lastWinWidth === curWinWidth && this.lastWinHeight === curWinHeight)
		{
			if (this.element_hidden)
			{
				jQuery(this.divElem).show();
				this.element_hidden = false;
			}
			
			return;
		}
			
		this.lastLeft = left;
		this.lastTop = top;
		this.lastRight = right;
		this.lastBottom = bottom;
		this.lastWinWidth = curWinWidth;
		this.lastWinHeight = curWinHeight;
		
		if (this.element_hidden)
		{
			jQuery(this.divElem).show();
			this.element_hidden = false;
		}
		
		var offx = Math.round(left) + jQuery(this.runtime.canvas).offset().left;
		var offy = Math.round(top) + jQuery(this.runtime.canvas).offset().top;
		jQuery(this.divElem).css("position", "absolute");
		jQuery(this.divElem).offset({left: offx, top: offy});
		jQuery(this.divElem).width(Math.round(right - left));
		jQuery(this.divElem).height(Math.round(bottom - top));
		
	};


    instanceProto.index2Type = function (i) {
        if( i === 0 ){
            return 'category'
        }
    }
	
	// only called if a layout object
	instanceProto.draw = function(ctx)
	{
	};
	
	instanceProto.drawGL = function(glw)
	{
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "EChart",
			"properties": []
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetVisible = function (vis)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.visible = (vis !== 0);
	};
	
	Acts.prototype.SetEnabled = function (en)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.inputElem.disabled = (en === 0);
	};


    Acts.prototype.Refresh = function ()
    {
        if (this.runtime.isDomFree || this.myChart )
            return;

        this.myChart.setOption(this.option);
    };

    Acts.prototype.setAxis = function (xa, type, gap, data, tag )
    {
        if (this.runtime.isDomFree || this.option )
            return;

        if( !this.option.legend ){
            this.option.legend = {
                show : true,
                data : []
            };
        }

        if( !this.option.legend.data){
            this.option.legend.data = [];
        }
        this.option.legend.data.push(name);

        var a = {};
        if( type >= 0 ){
            a.type = this.axistype[type];
        }
        else{
            a.type = (xa===0) ? this.axistype[0] :  this.axistype[1];
        }

        // 添加轴数据



        if( xa === 0 ){  // X轴
           this.option.xAxis = a;
        }
        else{ // Y轴
            this.option.xAxis = a;
        }





    };

    Acts.prototype.AddLegend = function (ori, data )
    {
        if (this.runtime.isDomFree || this.option )
            return;

        if( !this.option.legend ){
            this.option.legend = {
                show : true,
                orient : ori === 0 ? 'horizontal' : 'vertical',
                data : []
            };
        }

        // 从Arr对象添加数据
        if( !data ){

        }

    };

    Acts.prototype.AddLegendItem = function (name )
    {
        if (this.runtime.isDomFree || this.option || this.option.legend )
            return;

        this.option.legend.data.push(name);
    };


	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};

    Exps.prototype.title = function ()
    {
        ret.set_string(this.option.title.text);
    };

    Exps.prototype.subtitle = function ()
    {
        ret.set_string(this.option.title.subtext);
    };

	pluginProto.exps = new Exps();

}());