// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.sjcat = function(runtime)
{
	this.runtime = runtime;
};

(function () {
    var behaviorProto = cr.behaviors.sjcat.prototype;

    /////////////////////////////////////
    // Behavior type class
    behaviorProto.Type = function (behavior, objtype) {
        this.behavior = behavior;
        this.objtype = objtype;
        this.runtime = behavior.runtime;
    };

    var behtypeProto = behaviorProto.Type.prototype;

    behtypeProto.onCreate = function () {
    };

    /////////////////////////////////////
    // Behavior instance class
    behaviorProto.Instance = function (type, inst) {
        this.type = type;
        this.behavior = type.behavior;
        this.inst = inst;				// associated object instance to modify
        this.runtime = type.runtime;
    };

    var behinstProto = behaviorProto.Instance.prototype;

    behinstProto.onCreate = function () {
        this.map = null;
        this.nextstep = [-1,-1];
        this.state = 0;  // 0=正常，1=逃跑，2=围住
    };

    behinstProto.onDestroy = function () {
    };


    behinstProto.tick = function () {
    };

    behinstProto.tick2 = function () {
    };

    behinstProto.getRound = function (a) {
        var mapsize = this.map.cx;
        var c = a[0], // 列数
            d = a[1]; // 行数
        a = [];
        c = 0 == d % 2 ? [
            [c - 1, d - 1],
            [c - 1, d],
            [c - 1, d + 1],
            [c, d + 1],
            [c + 1, d],
            [c, d - 1]
        ] : [
            [c, d - 1],
            [c - 1, d],
            [c, d + 1],
            [c + 1, d + 1],
            [c + 1, d],
            [c + 1, d - 1]
        ];
        // 剔除边界的无效格子
        for (d = 0; 6 > d; d++) {
            var e = c[d];
            0 <= e[0] && 0 <= e[1] && e[0] < mapsize && e[1] < mapsize && a.push(e)
        }
        return a;
    };

    behinstProto.findPath = function (a) {
        var c = this.findNode(a);
        var d = [];
        if (c)
            for (; c.prenode;) d.push(c), c = c.prenode;
        else
            d = this.getNear(a);
        return d.reverse();
    };

    behinstProto.containNode = function (a, b) {
        var n = a.length;
        if (n === 0) return false;

        for(var i=0; i<n; i++){
           if(a[i][0] === b[0] && a[i][1] === b[1] ){
               return true;
           }
        }
        return false;
    }

    behinstProto.findNode = function(a) {
        var c = [a];
        a = [a];
        for (var d, e = 0, f = 0, g = 0, k = 0, n, p;;) {
            f = a.length;
            if (0 == f) return null;
            for (var q = [], e = 0; e < f; e++) {
                n = a.shift();
                d = this.getRound(n);
                k = d.length;
                for (g = 0; g < k; g++)
                    if ( p=d[g], this.map.at(p[1],p[0],0)!== 0 ) c.push(p);
                    else if (!(this.containNode(c,p) || this.containNode(a,p))){
                        p.prenode = n;
                        if (this.isExit(p)) return p;
                        q.push(p);
                    }
                c.push(n);
            }
            a = q;
        }
    };

    behinstProto.getNear = function(a) {
        a = this.getRound(a);
        for (var c = a.length, d = 0; d < c; d++){
            if (this.map.at(a[d][1],a[d][0],0) === 0) return [a[d]];
        }
        return [];
    };

    behinstProto.isExit = function(a) {
        var mapsize = this.map.cx;
        return 0 == a[0] || 0 == a[1] || a[0] == mapsize - 1 || a[1] == mapsize - 1 ? !0 : !1
    };

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

    Cnds.prototype.IsExit = function ()
    {
        // 是否到达地图边缘
        return this.state === 1;
    };

    Cnds.prototype.IsClosed = function ()
    {
        // 是否到达地图边缘
        return this.state === 2;
    };

	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.FindPath = function (grid)
	{
        if(!grid || grid.instances.length == 0){
            this.nextstep = [-1,-1];
            this.state = 0;
            return;
        }

        this.map = grid.instances[0];
        // 查找精灵所在位置
        var a = [];
        for (var x = 0; x < this.map.cx; x++)
        {
            for (var y = 0; y < this.map.cy; y++)
            {
                if(this.map.at(x,y,0) === 2){
                    a[0] = y;
                    a[1] = x;
                    break;
                }
            }
            if(a.length > 0 ){
                break;
            }
        }

        if(a.length == 0){
            this.nextstep = [-1,-1];
            this.state = 0;
            return;
        }

        var d = this.findPath(a);
        if(d.length === 0 ){
            this.nextstep = [-1,-1];
            this.state = 2;  // 被围住
        }
        else{
            // 取第一个位置
            this.nextstep = d[0];
            this.state = this.isExit(this.nextstep) ? 1 : 0;  // 判断是否逃跑成功
            // 替换网格
            this.map.set(a[1],a[0],0,0);
            this.map.set(this.nextstep[1],this.nextstep[0],0,2);
        }
	};


    Acts.prototype.Reset = function () {
        this.map = null;
        this.nextstep = [-1,-1];
        this.state = 0;
    }

	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.NextCol = function (ret)
	{
        ret.set_int(this.nextstep[0] );
	};

    Exps.prototype.NextRow = function (ret)
    {
        ret.set_int(this.nextstep[1] );
    };
	
	Exps.prototype.State = function (ret)
	{
		ret.set_int(this.state);
	};

	behaviorProto.exps = new Exps();
	
}());