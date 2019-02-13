export class WebGL {
    public gl : WebGLRenderingContext;
    constructor(gl : WebGLRenderingContext) {
        this.gl = gl;
    }

    compileShader(shader : WebGLShader, code : string) : void {
        this.gl.shaderSource(shader, code);
        this.gl.compileShader(shader);
    
        if(!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
            console.log(this.gl.getShaderInfoLog(shader));
            throw new Error('compile error');
        }
    }
    
    createProgram(vertexCode : string, fragmentCode : string) : WebGLProgram {
        const vertexShader = this.gl.createShader(this.gl.VERTEX_SHADER)!;
        this.compileShader(vertexShader, vertexCode);
    
        const fragmentShader = this.gl.createShader(this.gl.FRAGMENT_SHADER)!;
        this.compileShader(fragmentShader, fragmentCode);
    
        const program = this.gl.createProgram()!;
        this.gl.attachShader(program, vertexShader);
        this.gl.attachShader(program, fragmentShader);
        this.gl.linkProgram(program);
    
        if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
            console.log(this.gl.getProgramInfoLog(program));
            throw new Error("program error");
        }
        return program;
    }
    
    createVertexBuffer(vertices : number[]) : WebGLBuffer {
        const buffer = this.gl.createBuffer()!;
    
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(vertices), this.gl.STATIC_DRAW);
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, null); //TODO いらないの?
    
        return buffer;
    }

    setAttribute(buffers : WebGLBuffer[], attLocations : number[], attStrides : number[] ) {
        for(let i in buffers){
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buffers[i]);        
            this.gl.enableVertexAttribArray(attLocations[i]);
            this.gl.vertexAttribPointer(attLocations[i], attStrides[i], this.gl.FLOAT, false, 0, 0);
        }
    }
    
}
