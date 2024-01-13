#pragma once

namespace ui
{
	// Color (RGBA)
	struct color
	{
		unsigned char r, g, b, a;

		color() { r = 0; g = 0; b = 0; a = 255; }

		color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		{
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}

		color Shade(float value)
		{
			return color(r * value, g * value, b * value, a);
		}

		color Constrast()
		{
			if (r * 0.299 + g * 0.587 + b * 0.114 > 186)
			{
				return color(0, 0, 0, 255);
			}
			else
			{
				return color(255, 255, 255, 255);
			}
		}

		glm::vec4 Normalize()
		{
			return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		color Interpolate(color color2, float value)
		{
			color interp;

			interp.r = (color2.r - r) * value + r;
			interp.g = (color2.g - g) * value + g;
			interp.b = (color2.b - b) * value + b;
			interp.a = (color2.a - a) * value + a;

			return interp;
		}

		static color FromVec4(glm::vec4 col)
		{
			return color((unsigned char)col.r, (unsigned char)col.g, (unsigned char)col.b, (unsigned char)col.a);
		}

		static color FromUInt(unsigned int color)
		{
			return FromVec4(glm::unpackUnorm4x8(color) * 255.0f);
		}
	};

	// Constant colors
	color black = color(0, 0, 0, 255);
	color white = color(255, 255, 255, 255);
	color grey = color(128, 128, 128, 255);
	color darkGrey = color(64, 64, 64, 255);
	color red = color(255, 0, 0, 255);
	color green = color(0, 255, 0, 255);
	color blue = color(0, 0, 255, 255);
	color yellow = color(255, 255, 0, 255);


	// Target Resolution
	unsigned int targetScrWidth = 1024;
	unsigned int targetScrHeight = 540;

	// Corner (for either the UI anchor or the UI position)
	enum corner
	{
		topLeft,
		bottomLeft,
		topRight,
		bottomRight,
		middle,
		//middleTop,
		//middleBottom,
		//middleLeft,
		//middleRight,
	};

	// Corner enum to actual point
	void GetAnchorPos(int width, int height, corner anchor, float& anchorPointX, float& anchorPointY)
	{
		if (anchor == topLeft)
		{
			anchorPointX = 0;
			anchorPointY = 0;
		}
		else if (anchor == bottomLeft)
		{
			anchorPointX = 0;
			anchorPointY = height;
		}
		else if (anchor == topRight)
		{
			anchorPointX = width;
			anchorPointY = 0;
		}
		else if (anchor == bottomRight)
		{
			anchorPointX = width;
			anchorPointY = height;
		}
		else if (anchor == middle)
		{
			anchorPointX = (float)width / 2;
			anchorPointY = (float)height / 2;
		}
		/*else if (anchor == middleTop)
		{
			anchorPointX = (float)width / 2;
			anchorPointY = 0;
		}
		else if (anchor == middleBottom)
		{
			anchorPointX = (float)width / 2;
			anchorPointY = height;
		}
		else if (anchor == middleLeft)
		{
			anchorPointX = 0;
			anchorPointY = (float)height / 2;
		}
		else if (anchor == middleRight)
		{
			anchorPointX = width;
			anchorPointY = (float)height / 2;
		}*/
	}

	// Mouse-rectangle intersection
	bool mouseRect(float x, float y, float minX, float minY, float maxX, float maxY)
	{
		return (x >= minX && x <= maxX) && (y >= minY && y <= maxY);
	}

	struct solid
	{
		// UI Data
		bool visible;

		color color;
		float depth;

		corner cornerPoint;
		corner anchorPoint;

		float relPosX, relPosY;
		float posX, posY;
		float sizeX, sizeY;

		float scaleRelPosX, scaleRelPosY;
		float scalePosX, scalePosY;
		float scaleSizeX, scaleSizeY;

		// Functions
		void ResizeSolid(unsigned int width, unsigned int height);
		void ChangePosition(ui::corner corner, float x, float y);
	};

	struct text
	{
		bool visible;

		unsigned int VBO;

		unsigned int vSize;
		float* vertices = NULL;

		// UI Data
		std::string string;
		float pixelSize;

		color color;
		float depth;

		corner cornerPoint;
		corner anchorPoint;

		float relPosX, relPosY;
		float posX, posY;
		float sizeX, sizeY;

		float scaleRelPosX, scaleRelPosY;
		float scalePosX, scalePosY;
		float scaleSizeX, scaleSizeY;

		// Functions
		void ResizeText(unsigned int width, unsigned int height);
		void ChangeText(std::string str);
		void DisposeText();

	};

	struct button
	{
		bool enable = true;

		color defaultCol;
		color mouseOverCol;
		color mousePressCol;
		
		color currentCol;

		bool first = false;
		bool pressed = false;

		int value;
		void (*onPress)(int);

		unsigned int buttonSolid;
		unsigned int buttonText;

		void update(double x, double y, bool mousePressed);
		void EnableRenderer(bool enable);
	};

	// TODO: Create a texture struct for rendering images (don't forget to include a color field)

	// UI variables
	glm::mat4 orthoProjection;
	std::vector<ui::solid> solids;
	std::vector<ui::text> texts;
	std::vector<ui::button> buttons;

    #pragma region solid

	// Solid Specifications
	/*
		uint VBO - Vertex Buffer Object for the UI Element
		uint VAO - Vertex Array Obkect for the UI Element

		uint vSize - The number of vertices * 3 ( Due to the 3 components in each vertex)
		float[] vertices - Array of vertices for the UI Element (has vSize elements)

		rgb color - color of the UI Element
		float depth - The depth (z-value) at which to render the UI Element

		corner cornerPoint - The corner considered in the relative position
		corner anchorPoint - The corner of the UI Element's anchor

		These values below are in terms of the target resolution

		float relPosX, relPosY - This is the relative position of the UI Element where the origin is the anchor point
		float posX, posY; - This is the absolute position of the UI Element. ( This is in terms of the top left corner )
		float sizeX, sizeY - This is the size of the UI Element

		These values below are in terms of the current resolution (they are scaled)

		float scaleRelPosX, scaleRelPosY - This is the relative position of the UI Element where the origin is the anchor point scaled to fit the current resolution
		float scalePosX, scalePosY - This is the absolute position of the UI Element scaled to fit the current resolution. ( This is in terms of the top left corner )
		float scaleSizeX, scaleSizeY - This is the size of the UI Element scaled to fit the current resolution

		NOTE: The Anchor is the origin of the relative position.
	*/

	// Solid Shader Program
	unsigned int solidProgram;

	// Uniforms
	unsigned int solidDepthUniform;
	unsigned int solidSizeUniform;
	unsigned int solidPositionUniform;
	unsigned int solidColorUniform;
	unsigned int solidProjectionUniform;

	unsigned int solidVSize = 3 * 6;
	static const float solidVertices[] =
	{
		1.0f, 0.0f, 0.0f, // top right
		1.0f, 1.0f, 0.0f, // bottom right
		0.0f, 0.0f, 0.0f, // top left
		1.0f, 1.0f, 0.0f, // bottom right
		0.0f, 1.0f, 0.0f, // bottom left
		0.0f, 0.0f, 0.0f, // top left
	};

	unsigned int solidVBO;
	unsigned int solidVAO;
	unsigned int solidBindingIndex = 0;

	void solid::ResizeSolid(unsigned int width, unsigned int height)
	{
		if (width == targetScrWidth && height == targetScrHeight)
		{
			scaleRelPosX = relPosX;
			scaleRelPosY = relPosY;

			scalePosX = posX;
			scalePosY = posY;

			scaleSizeX = sizeX;
			scaleSizeY = sizeY;
		}
		else
		{
			float scaleRatio = (float)height / targetScrHeight;

			float sizeXRatio = sizeX / sizeY;
			float relPosXRatio = (relPosX == 0 && relPosY == 0) ? 1 : relPosX / relPosY;

			scaleRelPosY = scaleRatio * relPosY;
			scaleRelPosX = scaleRelPosY * relPosXRatio;

			scaleSizeY = scaleRatio * sizeY;
			scaleSizeX = scaleSizeY * sizeXRatio;

			// Getting anchorPoint
			float anchorPointX, anchorPointY;
			GetAnchorPos(width, height, anchorPoint, anchorPointX, anchorPointY);

			// Scaling position
			if (cornerPoint == topLeft)
			{
				scalePosX = scaleRelPosX + anchorPointX;
				scalePosY = scaleRelPosY + anchorPointY;
			}
			else if (cornerPoint == topRight)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX;
				scalePosY = scaleRelPosY + anchorPointY;
			}
			else if (cornerPoint == bottomLeft)
			{
				scalePosX = scaleRelPosX + anchorPointX;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY;
			}
			else if (cornerPoint == bottomRight)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY;
			}
			else if (cornerPoint == middle)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX / 2;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY / 2;
			}
		}
	}

	void solid::ChangePosition(ui::corner corner, float x, float y)
	{
		cornerPoint = corner;
		relPosX = x;
		relPosY = y;

		// Getting anchorPoint
		float anchorPointX, anchorPointY;
		GetAnchorPos(ui::targetScrWidth, ui::targetScrHeight, anchorPoint, anchorPointX, anchorPointY);

		// Getting aboslute position
		if (corner == ui::topLeft)
		{
			posX = relPosX + anchorPointX;
			posY = relPosY + anchorPointY;
		}
		else if (corner == ui::topRight)
		{
			posX = relPosX + anchorPointX - sizeX;
			posY = relPosY + anchorPointY;
		}
		else if (corner == ui::bottomLeft)
		{
			posX = relPosX + anchorPointX;
			posY = relPosY + anchorPointY - sizeY;
		}
		else if (corner == ui::bottomRight)
		{
			posX = relPosX + anchorPointX - sizeX;
			posY = relPosY + anchorPointY - sizeY;
		}
		else if (corner == ui::middle)
		{
			posX = relPosX + anchorPointX - sizeX / 2;
			posY = relPosY + anchorPointY - sizeY / 2;
		}

		// Resize to current window
		ResizeSolid(game::width, game::height);
	}

	solid CreateSolid(color color, float depth, corner anchor, corner corner, float x, float y, float sizeX, float sizeY, bool visible = true)
	{
		ui::solid solidUI;

		solidUI.color = color;
		solidUI.depth = depth;
		solidUI.anchorPoint = anchor;
		solidUI.cornerPoint = corner;
		solidUI.relPosX = x;
		solidUI.relPosY = y;
		solidUI.sizeX = sizeX;
		solidUI.sizeY = sizeY;
		solidUI.visible = visible;

		// Getting anchorPoint
		float anchorPointX, anchorPointY;
		GetAnchorPos(ui::targetScrWidth, ui::targetScrHeight, anchor, anchorPointX, anchorPointY);

		// Getting aboslute position
		if (corner == ui::topLeft)
		{
			solidUI.posX = solidUI.relPosX + anchorPointX;
			solidUI.posY = solidUI.relPosY + anchorPointY;
		}
		else if (corner == ui::topRight)
		{
			solidUI.posX = solidUI.relPosX + anchorPointX - sizeX;
			solidUI.posY = solidUI.relPosY + anchorPointY;
		}
		else if (corner == ui::bottomLeft)
		{
			solidUI.posX = solidUI.relPosX + anchorPointX;
			solidUI.posY = solidUI.relPosY + anchorPointY - sizeY;
		}
		else if (corner == ui::bottomRight)
		{
			solidUI.posX = solidUI.relPosX + anchorPointX - sizeX;
			solidUI.posY = solidUI.relPosY + anchorPointY - sizeY;
		}
		else if (corner == ui::middle)
		{
			solidUI.posX = solidUI.relPosX + anchorPointX - sizeX / 2;
			solidUI.posY = solidUI.relPosY + anchorPointY - sizeY / 2;
		}

		// Resize to current window
		solidUI.ResizeSolid(game::width, game::height);

		return solidUI;
	}

	void InitSolids()
	{
		// Create solid shader
		solidProgram = shader::createShader("assets/shaders/solid.vert", "assets/shaders/solid.frag");

		// Find solid shader uniforms
		solidPositionUniform = glGetUniformLocation(solidProgram, "position");
		solidSizeUniform = glGetUniformLocation(solidProgram, "size");
		solidDepthUniform = glGetUniformLocation(solidProgram, "depth");
		solidColorUniform = glGetUniformLocation(solidProgram, "color");
		solidProjectionUniform = glGetUniformLocation(solidProgram, "projection");

		glUniformMatrix4fv(solidProjectionUniform, 1, GL_FALSE, glm::value_ptr(orthoProjection));

		// Generate and Bind solid VAO
		glGenVertexArrays(1, &solidVAO);
		glBindVertexArray(solidVAO);

		// Create VAO format and bind that to the solidBindingIndex
		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(0, solidBindingIndex);
		glEnableVertexAttribArray(0);

		// Unbind from the solid VAO
		glBindVertexArray(0);

		// Generate VBO
		glGenBuffers(1, &solidVBO);

		// Send Verties to GPU
		glBindBuffer(GL_ARRAY_BUFFER, solidVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * solidVSize, solidVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void DrawSolids()
	{
		shader::useShader(solidProgram);

		glBindVertexArray(solidVAO);

		glBindVertexBuffer(solidBindingIndex, solidVBO, 0, 3 * sizeof(float));

		for (ui::solid& sol : solids)
		{
			if (sol.visible)
			{
				// Maybe put if statement if this needs to be sorted
				glUniform4f(solidColorUniform, sol.color.r / 255.0f, sol.color.g / 255.0f, sol.color.b / 255.0f, sol.color.a / 255.0f);

				glUniform2f(solidPositionUniform, sol.scalePosX, sol.scalePosY);
				glUniform2f(solidSizeUniform, sol.scaleSizeX, sol.scaleSizeY);
				glUniform1f(solidDepthUniform, sol.depth);

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
	}

	void ResizeSolids(int width, int height)
	{
		shader::useShader(solidProgram);

		glUniformMatrix4fv(solidProjectionUniform, 1, GL_FALSE, glm::value_ptr(orthoProjection));

		for (ui::solid& sol : solids)
		{
			sol.ResizeSolid(width, height);
		}
	}

	void DisposeSolids()
	{
		shader::deleteShader(solidProgram);

		glDeleteVertexArrays(1, &solidVAO);
		glDeleteBuffers(1, &solidVBO);
	}

    #pragma endregion

	#pragma region text

	// Text Specifications
	/*
		pixelSize is the font height in pixels
	*/

	// Solid Shader Program
	unsigned int textProgram;

	// Uniforms
	unsigned int textDepthUniform;
	unsigned int textColorUniform;
	unsigned int textProjectionUniform;

	unsigned int textVAO;
	unsigned int textBindingIndex = 1;

	// Font Variables
	int bitmapWidth, bitmapHeight;
	int fontWidth = 30; // 13;
	int fontHeight = 48; // 24;
	int fontPerRow = 16;
	int fontPerColumn = 16;
	unsigned int fontTexture;

	void text::ResizeText(unsigned int width, unsigned int height)
	{
		if (width == ui::targetScrWidth && height == ui::targetScrHeight)
		{
			scaleRelPosX = relPosX;
			scaleRelPosY = relPosY;

			scalePosX = posX;
			scalePosY = posY;

			scaleSizeX = sizeX;
			scaleSizeY = sizeY;
		}
		else
		{
			float scaleRatio = (float)height / ui::targetScrHeight;

			float sizeXRatio = sizeX / sizeY;
			float relPosXRatio = (relPosX == 0 && relPosY == 0) ? 1 : relPosX / relPosY;

			scaleRelPosY = scaleRatio * relPosY;
			scaleRelPosX = scaleRelPosY * relPosXRatio;

			scaleSizeY = scaleRatio * sizeY;
			scaleSizeX = scaleSizeY * sizeXRatio;

			// Getting anchorPoint
			float anchorPointX, anchorPointY;
			ui::GetAnchorPos(width, height, anchorPoint, anchorPointX, anchorPointY);

			// Scaling posisiton
			if (cornerPoint == ui::topLeft)
			{
				scalePosX = scaleRelPosX + anchorPointX;
				scalePosY = scaleRelPosY + anchorPointY;
			}
			else if (cornerPoint == ui::topRight)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX;
				scalePosY = scaleRelPosY + anchorPointY;
			}
			else if (cornerPoint == ui::bottomLeft)
			{
				scalePosX = scaleRelPosX + anchorPointX;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY;
			}
			else if (cornerPoint == ui::bottomRight)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY;
			}
			else if (cornerPoint == ui::middle)
			{
				scalePosX = scaleRelPosX + anchorPointX - scaleSizeX / 2;
				scalePosY = scaleRelPosY + anchorPointY - scaleSizeY / 2;
			}
		}

		float textScale = pixelSize / fontHeight;
		float scaleRatio = (float)height / ui::targetScrHeight;
		int length = string.length();

		for (int i = 0; i < length; i++)
		{
			// Verticies
			{
				// top right
				vertices[i * 24 + 0] = scalePosX + (textScale * fontWidth * (1 + i)) * scaleRatio;
				vertices[i * 24 + 1] = scalePosY;

				// bottom right
				vertices[i * 24 + 4] = scalePosX + (textScale * fontWidth * (1 + i)) * scaleRatio;
				vertices[i * 24 + 5] = scalePosY + (fontHeight * textScale) * scaleRatio;

				// top left
				vertices[i * 24 + 8] = scalePosX + (textScale * fontWidth * i) * scaleRatio;
				vertices[i * 24 + 9] = scalePosY;

				// bottom right
				vertices[i * 24 + 12] = scalePosX + (textScale * fontWidth * (1 + i)) * scaleRatio;
				vertices[i * 24 + 13] = scalePosY + (fontHeight * textScale) * scaleRatio;

				// bottom left
				vertices[i * 24 + 16] = scalePosX + (textScale * fontWidth * i) * scaleRatio;
				vertices[i * 24 + 17] = scalePosY + (fontHeight * textScale) * scaleRatio;

				// top left
				vertices[i * 24 + 20] = scalePosX + (textScale * fontWidth * i) * scaleRatio;
				vertices[i * 24 + 21] = scalePosY;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vSize, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// TODO: Keep a watch on this function, there might be some errors created from it
	void text::ChangeText(std::string str)
	{
		// Old string length
		int oldLength = string.length();

		string = str;
		int length = str.length();
		float textScale = pixelSize / fontHeight;
		sizeX = length * (fontWidth * textScale);
		sizeY = fontHeight * textScale;

		// Getting anchorPoint
		float anchorPointX, anchorPointY;
		GetAnchorPos(ui::targetScrWidth, ui::targetScrHeight, anchorPoint, anchorPointX, anchorPointY);

		// Getting aboslute position
		if (cornerPoint == ui::topLeft)
		{
			posX = relPosX + anchorPointX;
			posY = relPosY + anchorPointY;
		}
		else if (cornerPoint == ui::topRight)
		{
			posX = relPosX + anchorPointX - sizeX;
			posY = relPosY + anchorPointY;
		}
		else if (cornerPoint == ui::bottomLeft)
		{
			posX = relPosX + anchorPointX;
			posY = relPosY + anchorPointY - sizeY;
		}
		else if (cornerPoint == ui::bottomRight)
		{
			posX = relPosX + anchorPointX - sizeX;
			posY = relPosY + anchorPointY - sizeY;
		}
		else if (cornerPoint == ui::middle)
		{
			posX = relPosX + anchorPointX - sizeX / 2;
			posY = relPosY + anchorPointY - sizeY / 2;
		}

		// 4 is vec4, 6 is for the 2 tris
		vSize = (4 * 6) * length;

		// Delete verticies and resize if new string is larger than old
		if (length > oldLength) 
		{
			delete[] vertices;
			vertices = new float[vSize];
		}
		else memset(vertices, 0, (4 * 6) * oldLength); // We memset to make sure data at end of vertices pointer gets deleted

		for (int i = 0; i < length; i++)
		{
			unsigned char ch = str[i];
			int charX = (ch) % fontPerRow;
			int charY = fontPerColumn - (ch - charX) / fontPerRow - 1;

			// Verticies + UVs
			{
				// i * scale represents the scale between each char
				// fontWidth * scale represents the width of the char
				// fontHeight * scale represents the length of the char

				// Vertex
				// Vertex
				// UV
				// UV

				// top right
				vertices[i * 24 + 0] = posX + textScale * fontWidth * (1 + i);
				vertices[i * 24 + 1] = posY;
				vertices[i * 24 + 2] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				vertices[i * 24 + 3] = (float)(fontHeight * (charY + 1)) / bitmapHeight;

				// bottom right
				vertices[i * 24 + 4] = posX + textScale * fontWidth * (1 + i);
				vertices[i * 24 + 5] = posY + fontHeight * textScale;
				vertices[i * 24 + 6] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				vertices[i * 24 + 7] = (float)(fontHeight * charY) / bitmapHeight;

				// top left
				vertices[i * 24 + 8] = posX + textScale * fontWidth * i;
				vertices[i * 24 + 9] = posY;
				vertices[i * 24 + 10] = (float)(charX * fontWidth) / bitmapWidth;
				vertices[i * 24 + 11] = (float)(fontHeight * (charY + 1)) / bitmapHeight;

				// bottom right
				vertices[i * 24 + 12] = posX + textScale * fontWidth * (1 + i);
				vertices[i * 24 + 13] = posY + fontHeight * textScale;
				vertices[i * 24 + 14] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				vertices[i * 24 + 15] = (float)(charY * fontHeight) / bitmapHeight;

				// bottom left
				vertices[i * 24 + 16] = posX + textScale * fontWidth * i;
				vertices[i * 24 + 17] = posY + fontHeight * textScale;
				vertices[i * 24 + 18] = (float)(fontWidth * charX) / bitmapWidth;
				vertices[i * 24 + 19] = (float)(fontHeight * charY) / bitmapHeight;

				// top left
				vertices[i * 24 + 20] = posX + textScale * fontWidth * i;
				vertices[i * 24 + 21] = posY;
				vertices[i * 24 + 22] = (float)(fontWidth * charX) / bitmapWidth;
				vertices[i * 24 + 23] = (float)(fontHeight * (charY + 1)) / bitmapHeight;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// TODO: Put glSubBufferData if length <= oldLength so we don't need to call costly glBufferData fucntion
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vSize, vertices, GL_DYNAMIC_DRAW);

		ResizeText(game::width, game::height);
	}

	text CreateText(std::string str, float pixelSize, color color, float depth, corner anchor, corner corner, float x, float y, bool visible = true)
	{
		text textUI;

		textUI.string = str;
		textUI.color = color;
		textUI.depth = depth;
		textUI.pixelSize = pixelSize;
		textUI.anchorPoint = anchor;
		textUI.cornerPoint = corner;
		textUI.relPosX = x;
		textUI.relPosY = y;
		textUI.visible = visible;

		int length = str.length();
		float textScale = pixelSize / fontHeight;
		textUI.sizeX = length * (fontWidth * textScale);
		textUI.sizeY = fontHeight * textScale;

		// Getting anchorPoint
		float anchorPointX, anchorPointY;
		GetAnchorPos(ui::targetScrWidth, ui::targetScrHeight, anchor, anchorPointX, anchorPointY);

		// Getting aboslute position
		if (corner == ui::topLeft)
		{
			textUI.posX = textUI.relPosX + anchorPointX;
			textUI.posY = textUI.relPosY + anchorPointY;
		}
		else if (corner == ui::topRight)
		{
			textUI.posX = textUI.relPosX + anchorPointX - textUI.sizeX;
			textUI.posY = textUI.relPosY + anchorPointY;
		}
		else if (corner == ui::bottomLeft)
		{
			textUI.posX = textUI.relPosX + anchorPointX;
			textUI.posY = textUI.relPosY + anchorPointY - textUI.sizeY;
		}
		else if (corner == ui::bottomRight)
		{
			textUI.posX = textUI.relPosX + anchorPointX - textUI.sizeX;
			textUI.posY = textUI.relPosY + anchorPointY - textUI.sizeY;
		}
		else if (corner == ui::middle)
		{
			textUI.posX = textUI.relPosX + anchorPointX - textUI.sizeX / 2;
			textUI.posY = textUI.relPosY + anchorPointY - textUI.sizeY / 2;
		}

		// 4 is vec4, 6 is for the 2 tris
		textUI.vSize = (4 * 6) * length;
		textUI.vertices = new float[textUI.vSize];

		for (int i = 0; i < length; i++)
		{
			unsigned char ch = str[i];
			int charX = (ch) % fontPerRow;
			int charY = fontPerColumn - (ch - charX) / fontPerRow - 1;

			// Verticies + UVs
			{
				// i * scale represents the scale between each char
				// fontWidth * scale represents the width of the char
				// fontHeight * scale represents the length of the char

				// Vertex
				// Vertex
				// UV
				// UV

				// top right
				textUI.vertices[i * 24 + 0] = textUI.posX + textScale * fontWidth * (1 + i);
				textUI.vertices[i * 24 + 1] = textUI.posY;
				textUI.vertices[i * 24 + 2] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				textUI.vertices[i * 24 + 3] = (float)(fontHeight * (charY + 1)) / bitmapHeight;

				// bottom right
				textUI.vertices[i * 24 + 4] = textUI.posX + textScale * fontWidth * (1 + i);
				textUI.vertices[i * 24 + 5] = textUI.posY + fontHeight * textScale;
				textUI.vertices[i * 24 + 6] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				textUI.vertices[i * 24 + 7] = (float)(fontHeight * charY) / bitmapHeight;

				// top left
				textUI.vertices[i * 24 + 8] = textUI.posX + textScale * fontWidth * i;
				textUI.vertices[i * 24 + 9] = textUI.posY;
				textUI.vertices[i * 24 + 10] = (float)(charX * fontWidth) / bitmapWidth;
				textUI.vertices[i * 24 + 11] = (float)(fontHeight * (charY + 1)) / bitmapHeight;

				// bottom right
				textUI.vertices[i * 24 + 12] = textUI.posX + textScale * fontWidth * (1 + i);
				textUI.vertices[i * 24 + 13] = textUI.posY + fontHeight * textScale;
				textUI.vertices[i * 24 + 14] = (float)(fontWidth * (charX + 1)) / bitmapWidth;
				textUI.vertices[i * 24 + 15] = (float)(charY * fontHeight) / bitmapHeight;

				// bottom left
				textUI.vertices[i * 24 + 16] = textUI.posX + textScale * fontWidth * i;
				textUI.vertices[i * 24 + 17] = textUI.posY + fontHeight * textScale;
				textUI.vertices[i * 24 + 18] = (float)(fontWidth * charX) / bitmapWidth;
				textUI.vertices[i * 24 + 19] = (float)(fontHeight * charY) / bitmapHeight;

				// top left
				textUI.vertices[i * 24 + 20] = textUI.posX + textScale * fontWidth * i;
				textUI.vertices[i * 24 + 21] = textUI.posY;
				textUI.vertices[i * 24 + 22] = (float)(fontWidth * charX) / bitmapWidth;
				textUI.vertices[i * 24 + 23] = (float)(fontHeight * (charY + 1)) / bitmapHeight;
			}
		}
		
		glGenBuffers(1, &textUI.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, textUI.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textUI.vSize, textUI.vertices, GL_DYNAMIC_DRAW);

		textUI.ResizeText(game::width, game::height);

		return textUI;
	}

	void text::DisposeText()
	{
		glDeleteBuffers(1, &VBO);
		delete[] vertices;
	}

	void InitTexts()
	{
		// Create solid shader
		textProgram = shader::createShader("assets/shaders/text.vert", "assets/shaders/text.frag");

		// Find solid shader uniforms
		textDepthUniform = glGetUniformLocation(textProgram, "depth");
		textColorUniform = glGetUniformLocation(textProgram, "color");
		textProjectionUniform = glGetUniformLocation(textProgram, "projection");

		glUniformMatrix4fv(textProjectionUniform, 1, GL_FALSE, glm::value_ptr(orthoProjection));

		// Generate and Bind solid VAO
		glGenVertexArrays(1, &textVAO);
		glBindVertexArray(textVAO);

		// Create VAO format and bind that to the solidBindingIndex
		glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(0, textBindingIndex);
		glEnableVertexAttribArray(0);

		// Unbind from the solid VAO
		glBindVertexArray(0);

		// Create Font Texture
		glGenTextures(1, &fontTexture);
		glBindTexture(GL_TEXTURE_2D, fontTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Font Image
		int iwidth, iheight, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load("assets/fonts/courier48.png", &iwidth, &iheight, &nrChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		bitmapWidth = iwidth;
		bitmapHeight = iheight;
	}

	void DrawTexts()
	{
		shader::useShader(textProgram);

		glBindVertexArray(textVAO);

		for (text tex : texts)
		{
			if (tex.visible)
			{
				glUniform4f(textColorUniform, tex.color.r / 255.0f, tex.color.g / 255.0f, tex.color.b / 255.0f, tex.color.a / 255.0f);
				glUniform1f(textDepthUniform, tex.depth);

				glBindTexture(GL_TEXTURE_2D, fontTexture);
				glBindVertexBuffer(textBindingIndex, tex.VBO, 0, 4 * sizeof(float));

				glDrawArrays(GL_TRIANGLES, 0, tex.string.length() * 6);

				// glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

	void ResizeTexts(int width, int height)
	{
		shader::useShader(textProgram);

		glUniformMatrix4fv(textProjectionUniform, 1, GL_FALSE, glm::value_ptr(orthoProjection));

		for (text &tex : texts)
		{
			tex.ResizeText(width, height);
		}
	}

	void DisposeTexts()
	{
		shader::deleteShader(textProgram);
		glDeleteVertexArrays(1, &textVAO);
		glDeleteTextures(1, &fontTexture);

		for (text &tex : texts)
		{
			tex.DisposeText();
		}
	}

	#pragma endregion

	#pragma region button

	void button::update(double x, double y, bool mousePressed)
	{
		solid* sol = &solids[buttonSolid];
		text* tex = &texts[buttonText];

		pressed = mousePressed;

		bool mouseIn = mouseRect(x, y,
			sol->scalePosX,
			sol->scalePosY,
			sol->scalePosX + sol->scaleSizeX,
			sol->scalePosY + sol->scaleSizeY);

		if (mouseIn)
		{
			if (mousePressed && !first)
			{
				first = true;

				currentCol = mousePressCol;

				onPress(value);
			}
			else if (!mousePressed && first)
			{
				first = false;

				currentCol = mouseOverCol;
			}
			else if (mousePressed && first)
			{
				currentCol = mousePressCol;
			}
			else
			{
				currentCol = mouseOverCol;
			}
		}
		else
		{
			currentCol = defaultCol;
		}

		sol->color = currentCol;
	}

	void button::EnableRenderer(bool enable)
	{
		solids[buttonSolid].visible = enable;
		texts[buttonText].visible = enable;
	}

	button CreateButton(color defCol, color overCol, color pressCol, color textCol, std::string string, float pixelSize, float depth, corner anchor, corner corner, float x, float y, float sizeX, float sizeY, void (*onPress)(int))
	{
		button buttonUI;

		buttonUI.defaultCol = defCol;
		buttonUI.mouseOverCol = overCol;
		buttonUI.mousePressCol = pressCol;

		buttonUI.defaultCol = defCol;

		buttonUI.onPress = onPress;

		solids.push_back(CreateSolid(defCol, depth, anchor, corner, x, y, sizeX, sizeY));
		buttonUI.buttonSolid = solids.size() - 1;

		int multX = 0, multY = 0;

		if (anchor == topLeft)
		{
			multX = 1;
			multY = 1;
		}
		else if (anchor == topRight)
		{
			multX = -1;
			multY = 1;
		}
		else if (anchor == bottomLeft)
		{
			multX = 1;
			multY = -1;
		}
		else if (anchor == bottomRight)
		{
			multX = -1;
			multY = -1;
		}
		// Not sure what to do for middle (?)

		texts.push_back(CreateText(string, pixelSize, textCol, depth + 1, anchor, corner::middle, 
			solids[buttonUI.buttonSolid].scaleRelPosX + multX * (solids[buttonUI.buttonSolid].scaleSizeX / 2),
			solids[buttonUI.buttonSolid].scaleRelPosY + multY * (solids[buttonUI.buttonSolid].scaleSizeY / 2)));
		buttonUI.buttonText = texts.size() - 1;

		return buttonUI;
	}

	button CreateButton(color defCol, std::string string, float pixelSize, float depth, corner anchor, corner corner, float x, float y, float sizeX, float sizeY, void (*onPress)(int))
	{
		return CreateButton(defCol, defCol.Shade(0.75), defCol.Shade(0.5), defCol.Constrast(), string, pixelSize, depth, anchor, corner, x, y, sizeX, sizeY, onPress);
	}

	void UpdateButtons()
	{
		double x = 0, y = 0;
		glfwGetCursorPos(game::window, &x, &y);

		bool isMousePressed = glfwGetMouseButton(game::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		for (button &button : buttons)
		{
			if (button.enable)
			{
				button.update(x, y, isMousePressed);
			}

			if (texts[button.buttonText].string == "rgb block")
			{
				unsigned char r = 255 * (0.5 + 0.5 * cos(game::currentTime + 0));
				unsigned char g = 255 * (0.5 + 0.5 * cos(game::currentTime + 2));
				unsigned char b = 255 * (0.5 + 0.5 * cos(game::currentTime + 4));

				button.defaultCol = ui::color(r, g, b, 255);
				button.mouseOverCol = button.defaultCol.Shade(0.75f);
				button.mousePressCol = button.defaultCol.Shade(0.5f);


				//solids[button.buttonSolid].color = ui::color(r, g, b, 255);
			}
		}
	}

	#pragma endregion

	// UI for choosing element
	int padding = 20;
	int innerPadding = 10;
	int selectionButtonWidth = 175;
	int selectionButtonHeight = 26;

	bool choosingElement = false; // In Element Choosing UI or not
	int elementSelectionBase;
	int elementSelectionText;

	int currentCategory = 0;
	int categoryStart; // Start Index of Cateogry Buttons
	int categoryEnd; // End Index of Cateogry Buttons

	std::vector<int> categoryElementStart; // Start Index of Element Buttons for each category
	std::vector<int> categoryElementEnd; // End Index of Element Buttons for each category
	
	int menuPadding = 20;

	// Main Menu Base and Text toggle
	enum mainMenuChoice
	{
		none, start, settings, about
	};
	mainMenuChoice currentMainMenu = none;

	// Main Menu Elements
	int splashScreen; // Index of splash screen
	int sand3dText; // Index of sand3d text
	int startButton; // Index of start game button
	int settingsButton; // Index of settings button
	int aboutButton; // Index of about button
	int closeButton; // Index of close button
	int mainMenuBase; // index of menu selection base 
	int mainMenuText; // index of menu selection text
	// int discordButton; // Index of discord server link 

	// Game Loop Elements
	int crossHair; // Index of crosshair
	int selectionMenuBase; // Index of element selection menu base
	int selectionMenuText; // Index of element selection menu text

	// Implement paging somehow
	int currentPage = 1;

	// Select Cateogry
	void SelectCategory(int value)
	{
		bool notFound = true;

		for (int i = 0; i < buttons.size(); i++)
		{
			if (buttons[i].value == currentCategory)
			{
				buttons[i].defaultCol = color(72, 50, 72, 255);
				buttons[i].mouseOverCol = buttons[i].defaultCol.Shade(0.75f);
				buttons[i].mousePressCol = buttons[i].defaultCol.Shade(0.5f);
				break;
			}
		}

		if (categoryElementStart[currentCategory] <= categoryElementEnd[currentCategory])
		{
			for (int j = categoryElementStart[currentCategory]; j <= categoryElementEnd[currentCategory]; j++)
			{
				buttons[j].enable = false;
				buttons[j].EnableRenderer(false);
			}
		}

		currentCategory = value;
		
		for (int i = 0; i < buttons.size(); i++)
		{
			if (buttons[i].value == currentCategory)
			{
				notFound = false;
				buttons[i].defaultCol = color(149, 92, 163, 255);
				buttons[i].mouseOverCol = buttons[i].defaultCol.Shade(0.75f);
				buttons[i].mousePressCol = buttons[i].defaultCol.Shade(0.5f);
				break;
			}
		}

		if (categoryElementStart[currentCategory] <= categoryElementEnd[currentCategory])
		{
			for (int j = categoryElementStart[currentCategory]; j <= categoryElementEnd[currentCategory]; j++)
			{
				buttons[j].enable = true;
				buttons[j].EnableRenderer(true);
			}
		}

		std::cout << "The current category is " << elements::categoryList[currentCategory] << " (" << currentCategory << ")" << std::endl;
	}

	// Select Element
	void SelectElement(int value)
	{
		game::elementSelect = value;

		std::cout << elements::descriptionList[game::elementSelect].name << std::endl;
	}

	// Enable or Disable the element selection menu
	void EnableMenu(bool enable)
	{
		if (enable)
		{
			// Making mouse move around freely
			glfwSetInputMode(game::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			// Make crosshair invisible 
			solids[crossHair].visible = false;

			// Make main menu base and text visible
			solids[elementSelectionBase].visible = true;
			texts[elementSelectionText].visible = true;

			// Make all category buttons visible
			for (int j = categoryStart; j <= categoryEnd; j++)
			{
				buttons[j].enable = true;
				buttons[j].EnableRenderer(true);
			}

			// Make all element buttons visible 
			if (categoryElementStart[currentCategory] <= categoryElementEnd[currentCategory])
			{
				for (int j = categoryElementStart[currentCategory]; j <= categoryElementEnd[currentCategory]; j++)
				{
					buttons[j].enable = true;
					buttons[j].EnableRenderer(true);
				}
			}

			// Just to look a little better
			glfwSetCursorPos(game::window, game::currentWidth / 2, game::currentHeight / 2);
		}
		else
		{
			// Making mouse stay in the middle and be invisible
			glfwSetInputMode(game::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// Make crosshair visible
			solids[crossHair].visible = true;

			// Make main menu base and text invisible
			solids[elementSelectionBase].visible = false;
			texts[elementSelectionText].visible = false;

			// Make all category buttons invisible
			for (int j = categoryStart; j <= categoryEnd; j++)
			{
				buttons[j].enable = false;
				buttons[j].EnableRenderer(false);
			}

			// Make all element buttons Invisible 
			if (categoryElementStart[currentCategory] <= categoryElementEnd[currentCategory])
			{
				for (int j = categoryElementStart[currentCategory]; j <= categoryElementEnd[currentCategory]; j++)
				{
					buttons[j].enable = false;
					buttons[j].EnableRenderer(false);
				}
			}

			// This makes sure there is no jitter when exiting out of element menu
			game::resetMouseOffset = true;
		}
	}

	void EnableMainMenu(bool enable)
	{
		texts[sand3dText].visible = enable;

		buttons[startButton].enable = enable;
		buttons[startButton].EnableRenderer(enable);
		buttons[settingsButton].enable = enable;
		buttons[settingsButton].EnableRenderer(enable);
		buttons[aboutButton].enable = enable;
		buttons[aboutButton].EnableRenderer(enable);
		buttons[closeButton].enable = enable;
		buttons[closeButton].EnableRenderer(enable);

		solids[mainMenuBase].visible = false;
		texts[mainMenuText].visible = false;
	}

	// Main Menu Button Methods

	void Start(int value)
	{
		if (currentMainMenu == start)
		{
			solids[mainMenuBase].visible = false;
			texts[mainMenuText].visible = false;

			currentMainMenu = none;
		}
		else
		{
			solids[mainMenuBase].visible = true;
			texts[mainMenuText].visible = true;
			texts[mainMenuText].ChangeText("start");

			currentMainMenu = start;
		}
	}

	void Settings(int value)
	{
		if (currentMainMenu == settings)
		{
			solids[mainMenuBase].visible = false;
			texts[mainMenuText].visible = false;

			currentMainMenu = none;
		}
		else
		{
			solids[mainMenuBase].visible = true;
			texts[mainMenuText].visible = true;
			texts[mainMenuText].ChangeText("settings");

			currentMainMenu = settings;
		}
	}

	void About(int value)
	{
		if (currentMainMenu == about)
		{
			solids[mainMenuBase].visible = false;
			texts[mainMenuText].visible = false;
		}
		else
		{
			solids[mainMenuBase].visible = true;
			texts[mainMenuText].visible = true;
			texts[mainMenuText].ChangeText("about");

			currentMainMenu = about;
		}
	}

	void Exit(int value)
	{
		glfwSetWindowShouldClose(game::window, 1);
	}

	void NewSave(int value)
	{
		int count = 0;
		for (const auto& entry : std::filesystem::directory_iterator("assets/saves/"))
		{
			std::string pth = entry.path().string();
			int n = std::stoi(pth.substr(pth.find_first_of("0123456789")));
			if (n > count) count = n;
		}

		std::ofstream file("assets/saves/save " + std::to_string(count + 1) + ".s3d");
		// Write to the file
		file << "This is the save file of " << (count + 1);

		// Close the file
		file.close();
	}

	// TODO: Add create new save function for new button
	// TODO: Add system in text to start new line in text with '/n'

	// UI Functions

	void InitUI()
	{
		// Set the orthographic projection
		orthoProjection = glm::ortho(0.0f, (float)game::width, (float)game::height, 0.0f, -10.0f, 10.0f);

		InitSolids();
		InitTexts();

		// Crosshair
		solids.push_back(CreateSolid(color(255, 0, 0, 128), 1, ui::corner::middle, ui::corner::middle, 0, 0, 4, 4));
		crossHair = solids.size() - 1;

		// Element Selection Menu
		solids.push_back(CreateSolid(color(50, 74, 95, 255), 1, ui::corner::topLeft, ui::corner::topLeft, padding, padding, game::width - selectionButtonWidth - padding * 3, game::height - padding * 2));
		texts.push_back(CreateText("Element Selection", 20, color(0, 0, 0, 255), 2, ui::corner::topLeft, ui::corner::middle, padding + solids[1].scaleSizeX / 2, 5 + padding + selectionButtonHeight / 2));
		elementSelectionBase = solids.size() - 1; // Index of Base
		elementSelectionText = texts.size() - 1; // Index of Text

		// Add to the x to make cenetred in the element selection ui
		int elementSelectOffset = ((game::width - selectionButtonWidth - 3 * padding) - (4 * selectionButtonWidth + 3 * innerPadding)) / 2;

		color eggplant = color(72, 50, 72, 255);

		// Categories Select
		categoryStart = buttons.size();
		for (int i = 0; i < elements::categoryList.size(); i++)
		{
			buttons.push_back(CreateButton(
				eggplant,
				eggplant.Shade(0.75),
				eggplant.Shade(0.5),
				eggplant.Constrast(),
				elements::categoryList[i],
				20, 1,
				ui::corner::topLeft, ui::corner::topLeft,
				padding + elementSelectOffset, padding + (innerPadding + selectionButtonHeight) * (i + 1), selectionButtonWidth, selectionButtonHeight, SelectCategory));

			buttons[buttons.size() - 1].value = i;
		}
		categoryEnd = buttons.size() - 1;

		// Element Buttons for each category
		for (int i = 0; i < elements::categoryList.size(); i++) // i represents the current category in integer form
		{
			int elementNum = 0;

			categoryElementStart.push_back(buttons.size());
			for (int j = 0; j < elements::descriptionList.size(); j++)
			{
				game::elementDescription desc = elements::descriptionList[j];

				if (i == desc.category && !desc.hidden)
				{
					color color1 = color::FromUInt(elements::definitions[j].color1);
					color color2 = color::FromUInt(elements::definitions[j].color2);
					color interp = color1.Interpolate(color2, util::randXOR() / (float)UINT_MAX);

					int x = elementNum % 3;
					int y = (elementNum - x) / 3;

					int posX = padding + (innerPadding + selectionButtonWidth) * (x + 1) + elementSelectOffset;
					int posY = padding + (innerPadding + selectionButtonHeight) * (y + 1);

					buttons.push_back(CreateButton(
						interp,
						interp.Shade(0.75f),
						interp.Shade(0.5f),
						interp.Constrast(),
						desc.name,
						20, 1,
						ui::corner::topLeft, ui::corner::topLeft,
						posX, posY, 175, 26, SelectElement));

					buttons[buttons.size() - 1].value = j;

					elementNum++;
				}
			}

			categoryElementEnd.push_back(buttons.size() - 1);

		}

		// show only solid
		for (int i = 0; i < elements::categoryList.size(); i++)
		{
			if (i == 0) continue; // Keep solid

			for (int j = categoryElementStart[i]; j <= categoryElementEnd[i]; j++)
			{
				buttons[j].enable = false;
				buttons[j].EnableRenderer(false);
			}
		}

		EnableMenu(choosingElement);

		// Main Menu UI Elements

		// Create Splash Screen
		solids.push_back(CreateSolid(color(0, 128, 128, 255), 1, ui::corner::middle, ui::corner::middle, 0, 0, 100, 100));
		splashScreen = solids.size() - 1;
		
		// TODO: Make the texture UI element and replace this text with that
		// 38 pixels below the top center ((height / 2) - 200 - (64 / 2))
		texts.push_back(CreateText("sand3d", 64, color(255, 0, 0, 255), 2, ui::corner::middle, ui::corner::middle, 0, -200));
		sand3dText = texts.size() - 1;

		buttons.push_back(CreateButton(eggplant, "play", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
			menuPadding, menuPadding + 150 + (selectionButtonHeight + innerPadding) * 0, selectionButtonWidth, selectionButtonHeight, Start));
		startButton = buttons.size() - 1;

		buttons.push_back(CreateButton( eggplant, "settings", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
			menuPadding, menuPadding + 150 + (selectionButtonHeight + innerPadding) * 1, selectionButtonWidth, selectionButtonHeight, Settings));
		settingsButton = buttons.size() - 1;

		buttons.push_back(CreateButton( eggplant, "about", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
			menuPadding, menuPadding + 150 + (selectionButtonHeight + innerPadding) * 2, selectionButtonWidth, selectionButtonHeight, About));
		aboutButton = buttons.size() - 1;

		buttons.push_back(CreateButton( red, "exit", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
			menuPadding, menuPadding + 150 + (selectionButtonHeight + innerPadding) * 3, 
			selectionButtonWidth, selectionButtonHeight, Exit));
		closeButton = buttons.size() - 1;

		solids.push_back(CreateSolid(eggplant, 1, ui::corner::topLeft, ui::corner::topLeft, 
			menuPadding + (selectionButtonWidth + innerPadding), menuPadding + 150, game::width - (2 * menuPadding + (selectionButtonWidth + innerPadding)), game::height - 2 * menuPadding - 150));
		mainMenuBase = solids.size() - 1;

		texts.push_back(CreateText("Start", 20, white, 2, ui::corner::topLeft, ui::corner::middle, menuPadding + (selectionButtonWidth + innerPadding) + solids[mainMenuBase].scaleSizeX / 2, menuPadding + 150 + 5 + selectionButtonHeight / 2));
		mainMenuText = texts.size() - 1;

		std::cout << (game::width - (2 * menuPadding + (selectionButtonWidth + innerPadding))) << std::endl;

		/*for (int i = 1; i < 3; i++)
		{
			for (int j = 1; j < 9; j++)
			{
				buttons.push_back(CreateButton(red, "test", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
					menuPadding + (selectionButtonWidth + innerPadding) * i + 34.5, menuPadding + 150 + (selectionButtonHeight + innerPadding) * j,
					selectionButtonWidth, selectionButtonHeight, Exit));
			}
		}*/
		
		/*for (int i = 0; i < 9; i++)
		{
			buttons.push_back(CreateButton(red, "test", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
				menuPadding + (selectionButtonWidth + innerPadding) * ((i % 2) + 1) + 34.5, menuPadding + 150 + (selectionButtonHeight + innerPadding) * ((i / 2) + 1),
				selectionButtonWidth, selectionButtonHeight, Exit));
		}*/
		int count = 0;
		for (const auto& entry : std::filesystem::directory_iterator("assets/saves/"))
		{
			buttons.push_back(CreateButton(red, entry.path().stem().string(), 20, 1, ui::corner::topLeft, ui::corner::topLeft,
				menuPadding + (selectionButtonWidth + innerPadding) * ((count % 2) + 1) + 34.5, menuPadding + 150 + (selectionButtonHeight + innerPadding) * ((count / 2) + 1),
				selectionButtonWidth, selectionButtonHeight, Exit));
			count++;
		}

		buttons.push_back(CreateButton(red, "new", 20, 1, ui::corner::topLeft, ui::corner::topLeft,
			menuPadding + (selectionButtonWidth + innerPadding) * 3 + 34.5, menuPadding + 150 + (selectionButtonHeight + innerPadding) * 8,
			selectionButtonWidth * 2 + innerPadding, selectionButtonHeight, NewSave));

		EnableMainMenu(true);
	}

	// This is a new commit added

	void DrawUI()
	{
		//solids[0].ChangePosition(corner::middle, 100 * sin(game::currentTime), 0);

		DrawSolids();

		DrawTexts();
	}

	void ResizeUI(unsigned int width, unsigned int height)
	{
		ResizeSolids(width, height);
		ResizeTexts(width, height);
	}

	void DisposeUI()
	{
		DisposeSolids();
		DisposeTexts();
	}
}