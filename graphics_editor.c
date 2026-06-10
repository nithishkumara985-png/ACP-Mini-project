#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// ==================== CONSTANTS ====================
#define CANVAS_WIDTH 60
#define CANVAS_HEIGHT 20
#define MAX_OBJECTS 100
#define EMPTY_CELL '_'
#define DRAW_CELL '*'

// ==================== ENUMS ====================
typedef enum {
    LINE = 1,
    RECTANGLE = 2,
    TRIANGLE = 3,
    CIRCLE = 4
} ShapeType;

// ==================== STRUCTURES ====================
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} Coordinates;

typedef struct {
    int id;
    ShapeType shape_type;
    Coordinates coords;
    int radius;
    int is_visible;
    char shape_name[20];
} GraphicObject;

typedef struct {
    char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
    int total_objects;
    GraphicObject objects[MAX_OBJECTS];
    int next_object_id;
} Canvas;

// ==================== GLOBAL CANVAS ====================
Canvas canvas;

// ==================== FUNCTION PROTOTYPES ====================
void initializeCanvas(void);
void clearCanvas(void);
void displayCanvas(void);
void drawPixel(int x, int y);
void drawLine(int x1, int y1, int x2, int y2);
void drawRectangle(int x1, int y1, int x2, int y2);
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void drawCircle(int cx, int cy, int radius);
void addObject(ShapeType type, int x1, int y1, int x2, int y2, int x3, int y3, int radius);
void deleteObject(int object_id);
void modifyObject(int object_id);
void redrawCanvas(void);
void listObjects(void);
void saveCanvasToFile(const char *filename);
void loadCanvasFromFile(const char *filename);
void displayMenu(void);
int validateCoordinates(int x, int y);

// ==================== INITIALIZATION FUNCTIONS ====================

/*
 * initializeCanvas: Initializes the canvas with empty cells and resets object list
 * Time Complexity: O(h*w) where h=height, w=width
 * Space Complexity: O(1)
 */
void initializeCanvas(void) {
    int i, j;
    
    // Initialize canvas with empty cells
    for (i = 0; i < CANVAS_HEIGHT; i++) {
        for (j = 0; j < CANVAS_WIDTH; j++) {
            canvas.canvas[i][j] = EMPTY_CELL;
        }
    }
    
    // Reset object list
    canvas.total_objects = 0;
    canvas.next_object_id = 1;
    
    printf("\n✓ Canvas initialized successfully!\n");
}

/*
 * clearCanvas: Clears the canvas but maintains the object list
 * Time Complexity: O(h*w)
 * Space Complexity: O(1)
 */
void clearCanvas(void) {
    int i, j;
    
    for (i = 0; i < CANVAS_HEIGHT; i++) {
        for (j = 0; j < CANVAS_WIDTH; j++) {
            canvas.canvas[i][j] = EMPTY_CELL;
        }
    }
    
    printf("\n✓ Canvas cleared successfully!\n");
}

// ==================== DRAWING PRIMITIVES ====================

/*
 * validateCoordinates: Checks if coordinates are within canvas bounds
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
int validateCoordinates(int x, int y) {
    return (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT);
}

/*
 * drawPixel: Draws a pixel at (x, y) if within bounds
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
void drawPixel(int x, int y) {
    if (validateCoordinates(x, y)) {
        canvas.canvas[y][x] = DRAW_CELL;
    }
}

/*
 * drawLine: Uses Bresenham's Line Algorithm to draw a line
 * Time Complexity: O(max(|x2-x1|, |y2-y1|))
 * Space Complexity: O(1)
 */
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int x = x1, y = y1;
    
    while (1) {
        drawPixel(x, y);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

/*
 * drawRectangle: Draws a rectangle using four lines
 * Time Complexity: O(width + height)
 * Space Complexity: O(1)
 */
void drawRectangle(int x1, int y1, int x2, int y2) {
    // Ensure x1 < x2 and y1 < y2
    int minX = (x1 < x2) ? x1 : x2;
    int maxX = (x1 > x2) ? x1 : x2;
    int minY = (y1 < y2) ? y1 : y2;
    int maxY = (y1 > y2) ? y1 : y2;
    
    // Draw four sides
    drawLine(minX, minY, maxX, minY); // Top
    drawLine(maxX, minY, maxX, maxY); // Right
    drawLine(maxX, maxY, minX, maxY); // Bottom
    drawLine(minX, maxY, minX, minY); // Left
}

/*
 * drawTriangle: Draws a triangle using three lines
 * Time Complexity: O(max side length)
 * Space Complexity: O(1)
 */
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(x1, y1, x2, y2); // Side 1
    drawLine(x2, y2, x3, y3); // Side 2
    drawLine(x3, y3, x1, y1); // Side 3
}

/*
 * drawCircle: Uses Midpoint Circle Algorithm to draw a circle
 * Time Complexity: O(radius²)
 * Space Complexity: O(1)
 */
void drawCircle(int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int d = 3 - 2 * radius; // Decision parameter
    
    while (x >= y) {
        // Draw 8 octants
        drawPixel(cx + x, cy + y);
        drawPixel(cx - x, cy + y);
        drawPixel(cx + x, cy - y);
        drawPixel(cx - x, cy - y);
        drawPixel(cx + y, cy + x);
        drawPixel(cx - y, cy + x);
        drawPixel(cx + y, cy - x);
        drawPixel(cx - y, cy - x);
        
        if (d < 0) {
            d = d + 4 * y + 6;
        } else {
            d = d + 4 * (y - x) + 10;
            x--;
        }
        y++;
    }
}

// ==================== OBJECT MANAGEMENT ====================

/*
 * addObject: Adds a new graphical object to the object list
 * Time Complexity: O(pixels drawn)
 * Space Complexity: O(1)
 */
void addObject(ShapeType type, int x1, int y1, int x2, int y2, int x3, int y3, int radius) {
    if (canvas.total_objects >= MAX_OBJECTS) {
        printf("\n✗ Cannot add more objects! Maximum limit reached.\n");
        return;
    }
    
    GraphicObject obj;
    obj.id = canvas.next_object_id++;
    obj.shape_type = type;
    obj.coords.x1 = x1;
    obj.coords.y1 = y1;
    obj.coords.x2 = x2;
    obj.coords.y2 = y2;
    obj.coords.x3 = x3;
    obj.coords.y3 = y3;
    obj.radius = radius;
    obj.is_visible = 1;
    
    // Set shape name
    switch (type) {
        case LINE:
            strcpy(obj.shape_name, "Line");
            drawLine(x1, y1, x2, y2);
            break;
        case RECTANGLE:
            strcpy(obj.shape_name, "Rectangle");
            drawRectangle(x1, y1, x2, y2);
            break;
        case TRIANGLE:
            strcpy(obj.shape_name, "Triangle");
            drawTriangle(x1, y1, x2, y2, x3, y3);
            break;
        case CIRCLE:
            strcpy(obj.shape_name, "Circle");
            drawCircle(x1, y1, radius);
            break;
        default:
            printf("\n✗ Invalid shape type!\n");
            return;
    }
    
    canvas.objects[canvas.total_objects] = obj;
    canvas.total_objects++;
    
    printf("\n✓ Object %d added successfully! (Type: %s)\n", obj.id, obj.shape_name);
}

/*
 * deleteObject: Removes an object by ID and redraws canvas
 * Time Complexity: O(n + redraw)
 * Space Complexity: O(1)
 */
void deleteObject(int object_id) {
    int i, found = 0;
    
    for (i = 0; i < canvas.total_objects; i++) {
        if (canvas.objects[i].id == object_id) {
            found = 1;
            // Shift remaining objects
            for (int j = i; j < canvas.total_objects - 1; j++) {
                canvas.objects[j] = canvas.objects[j + 1];
            }
            canvas.total_objects--;
            printf("\n✓ Object %d deleted successfully!\n", object_id);
            redrawCanvas();
            return;
        }
    }
    
    if (!found) {
        printf("\n✗ Object %d not found!\n", object_id);
    }
}

/*
 * modifyObject: Modifies an existing object's position and dimensions
 * Time Complexity: O(n + redraw)
 * Space Complexity: O(1)
 */
void modifyObject(int object_id) {
    int i, found = 0;
    
    for (i = 0; i < canvas.total_objects; i++) {
        if (canvas.objects[i].id == object_id) {
            found = 1;
            GraphicObject *obj = &canvas.objects[i];
            
            printf("\n--- Modify Object %d (%s) ---\n", object_id, obj->shape_name);
            printf("Current coordinates: (%d,%d) to (%d,%d)\n", 
                   obj->coords.x1, obj->coords.y1, obj->coords.x2, obj->coords.y2);
            
            printf("Enter new X1 (current %d): ", obj->coords.x1);
            scanf("%d", &obj->coords.x1);
            printf("Enter new Y1 (current %d): ", obj->coords.y1);
            scanf("%d", &obj->coords.y1);
            printf("Enter new X2 (current %d): ", obj->coords.x2);
            scanf("%d", &obj->coords.x2);
            printf("Enter new Y2 (current %d): ", obj->coords.y2);
            scanf("%d", &obj->coords.y2);
            
            if (obj->shape_type == TRIANGLE) {
                printf("Enter new X3 (current %d): ", obj->coords.x3);
                scanf("%d", &obj->coords.x3);
                printf("Enter new Y3 (current %d): ", obj->coords.y3);
                scanf("%d", &obj->coords.y3);
            } else if (obj->shape_type == CIRCLE) {
                printf("Enter new Radius (current %d): ", obj->radius);
                scanf("%d", &obj->radius);
            }
            
            printf("\n✓ Object modified successfully!\n");
            redrawCanvas();
            return;
        }
    }
    
    if (!found) {
        printf("\n✗ Object %d not found!\n", object_id);
    }
}

/*
 * redrawCanvas: Redraws entire canvas from object list
 * Time Complexity: O(n * pixels) where n = number of objects
 * Space Complexity: O(1)
 */
void redrawCanvas(void) {
    clearCanvas();
    
    for (int i = 0; i < canvas.total_objects; i++) {
        if (canvas.objects[i].is_visible) {
            GraphicObject *obj = &canvas.objects[i];
            
            switch (obj->shape_type) {
                case LINE:
                    drawLine(obj->coords.x1, obj->coords.y1, 
                             obj->coords.x2, obj->coords.y2);
                    break;
                case RECTANGLE:
                    drawRectangle(obj->coords.x1, obj->coords.y1, 
                                  obj->coords.x2, obj->coords.y2);
                    break;
                case TRIANGLE:
                    drawTriangle(obj->coords.x1, obj->coords.y1,
                                 obj->coords.x2, obj->coords.y2,
                                 obj->coords.x3, obj->coords.y3);
                    break;
                case CIRCLE:
                    drawCircle(obj->coords.x1, obj->coords.y1, obj->radius);
                    break;
            }
        }
    }
}

/*
 * listObjects: Displays all objects in the object list
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */
void listObjects(void) {
    if (canvas.total_objects == 0) {
        printf("\n--- No objects in the canvas ---\n");
        return;
    }
    
    printf("\n========== ALL OBJECTS ==========\n");
    printf("%-5s %-12s %-30s %-10s\n", "ID", "Type", "Coordinates", "Status");
    printf("=====================================\n");
    
    for (int i = 0; i < canvas.total_objects; i++) {
        GraphicObject *obj = &canvas.objects[i];
        char coords[50];
        
        switch (obj->shape_type) {
            case LINE:
            case RECTANGLE:
                sprintf(coords, "(%d,%d) to (%d,%d)", obj->coords.x1, obj->coords.y1,
                        obj->coords.x2, obj->coords.y2);
                break;
            case TRIANGLE:
                sprintf(coords, "(%d,%d),(%d,%d),(%d,%d)", 
                        obj->coords.x1, obj->coords.y1,
                        obj->coords.x2, obj->coords.y2,
                        obj->coords.x3, obj->coords.y3);
                break;
            case CIRCLE:
                sprintf(coords, "Center:(%d,%d), R:%d", 
                        obj->coords.x1, obj->coords.y1, obj->radius);
                break;
        }
        
        printf("%-5d %-12s %-30s %s\n", obj->id, obj->shape_name, coords,
               obj->is_visible ? "Visible" : "Hidden");
    }
    printf("=====================================\n");
}

// ==================== DISPLAY FUNCTION ====================

/*
 * displayCanvas: Displays the canvas with borders
 * Time Complexity: O(h*w)
 * Space Complexity: O(1)
 */
void displayCanvas(void) {
    int i, j;
    
    printf("\n");
    printf("╔");
    for (j = 0; j < CANVAS_WIDTH; j++) printf("═");
    printf("╗\n");
    
    for (i = 0; i < CANVAS_HEIGHT; i++) {
        printf("║");
        for (j = 0; j < CANVAS_WIDTH; j++) {
            printf("%c", canvas.canvas[i][j]);
        }
        printf("║ %2d\n", i);
    }
    
    printf("╚");
    for (j = 0; j < CANVAS_WIDTH; j++) printf("═");
    printf("╝\n");
    
    printf("  ");
    for (j = 0; j < CANVAS_WIDTH; j++) {
        if (j % 10 == 0) printf("%d", j / 10);
        else printf(" ");
    }
    printf("\n");
    printf("  ");
    for (j = 0; j < CANVAS_WIDTH; j++) {
        printf("%d", j % 10);
    }
    printf("\n\n");
    
    printf("Canvas Size: %d x %d\n", CANVAS_WIDTH, CANVAS_HEIGHT);
    printf("Total Objects: %d\n\n", canvas.total_objects);
}

// ==================== FILE OPERATIONS ====================

/*
 * saveCanvasToFile: Saves canvas to a text file
 * Time Complexity: O(h*w)
 * Space Complexity: O(1)
 */
void saveCanvasToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        printf("\n✗ Error: Cannot open file for writing!\n");
        return;
    }
    
    // Save canvas
    fprintf(file, "%d %d\n", CANVAS_HEIGHT, CANVAS_WIDTH);
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            fprintf(file, "%c", canvas.canvas[i][j]);
        }
        fprintf(file, "\n");
    }
    
    // Save objects
    fprintf(file, "%d\n", canvas.total_objects);
    for (int i = 0; i < canvas.total_objects; i++) {
        GraphicObject *obj = &canvas.objects[i];
        fprintf(file, "%d %d %d %d %d %d %d %d %d %d\n",
                obj->id, obj->shape_type, obj->coords.x1, obj->coords.y1,
                obj->coords.x2, obj->coords.y2, obj->coords.x3, obj->coords.y3,
                obj->radius, obj->is_visible);
    }
    
    fclose(file);
    printf("\n✓ Canvas saved to '%s' successfully!\n", filename);
}

/*
 * loadCanvasFromFile: Loads canvas from a text file
 * Time Complexity: O(h*w + n) where n = number of objects
 * Space Complexity: O(1)
 */
void loadCanvasFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        printf("\n✗ Error: File not found!\n");
        return;
    }
    
    int height, width;
    fscanf(file, "%d %d\n", &height, &width);
    
    if (height != CANVAS_HEIGHT || width != CANVAS_WIDTH) {
        printf("\n✗ Error: Canvas size mismatch!\n");
        fclose(file);
        return;
    }
    
    // Load canvas
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            fscanf(file, "%c", &canvas.canvas[i][j]);
        }
        fscanf(file, "\n");
    }
    
    // Load objects
    int total;
    fscanf(file, "%d\n", &total);
    canvas.total_objects = total;
    
    for (int i = 0; i < total; i++) {
        GraphicObject *obj = &canvas.objects[i];
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d\n",
               &obj->id, (int*)&obj->shape_type, &obj->coords.x1, &obj->coords.y1,
               &obj->coords.x2, &obj->coords.y2, &obj->coords.x3, &obj->coords.y3,
               &obj->radius, &obj->is_visible);
        
        switch (obj->shape_type) {
            case LINE:
                strcpy(obj->shape_name, "Line");
                break;
            case RECTANGLE:
                strcpy(obj->shape_name, "Rectangle");
                break;
            case TRIANGLE:
                strcpy(obj->shape_name, "Triangle");
                break;
            case CIRCLE:
                strcpy(obj->shape_name, "Circle");
                break;
        }
        
        if (obj->id >= canvas.next_object_id) {
            canvas.next_object_id = obj->id + 1;
        }
    }
    
    fclose(file);
    printf("\n✓ Canvas loaded from '%s' successfully!\n", filename);
}

// ==================== MENU DISPLAY ====================

/*
 * displayMenu: Shows the main menu options
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
void displayMenu(void) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║   2D GRAPHICS EDITOR - MAIN MENU       ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  1. Display Canvas                     ║\n");
    printf("║  2. Draw Line                          ║\n");
    printf("║  3. Draw Rectangle                     ║\n");
    printf("║  4. Draw Triangle                      ║\n");
    printf("║  5. Draw Circle                        ║\n");
    printf("║  6. Delete Object                      ║\n");
    printf("║  7. Modify Object                      ║\n");
    printf("║  8. List All Objects                   ║\n");
    printf("║  9. Clear Canvas                       ║\n");
    printf("║ 10. Save Canvas to File                ║\n");
    printf("║ 11. Load Canvas from File              ║\n");
    printf("║  0. Exit                               ║\n");
    printf("╚════════════════════════════════════════╝\n");
}

// ==================== MAIN FUNCTION ====================

int main(void) {
    int choice, x1, y1, x2, y2, x3, y3, radius, object_id;
    char filename[100];
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   WELCOME TO 2D GRAPHICS EDITOR       ║\n");
    printf("║   A Data Structures Lab Project        ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    initializeCanvas();
    
    while (1) {
        displayMenu();
        printf("\nEnter your choice (0-11): ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                // Display Canvas
                displayCanvas();
                break;
                
            case 2:
                // Draw Line
                printf("\n--- Draw Line ---\n");
                printf("Enter X1: ");
                scanf("%d", &x1);
                printf("Enter Y1: ");
                scanf("%d", &y1);
                printf("Enter X2: ");
                scanf("%d", &x2);
                printf("Enter Y2: ");
                scanf("%d", &y2);
                
                if (!validateCoordinates(x1, y1) || !validateCoordinates(x2, y2)) {
                    printf("\n✗ Coordinates out of bounds! Valid range: X(0-%d), Y(0-%d)\n",
                           CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1);
                } else {
                    addObject(LINE, x1, y1, x2, y2, 0, 0, 0);
                }
                break;
                
            case 3:
                // Draw Rectangle
                printf("\n--- Draw Rectangle ---\n");
                printf("Enter X1: ");
                scanf("%d", &x1);
                printf("Enter Y1: ");
                scanf("%d", &y1);
                printf("Enter X2: ");
                scanf("%d", &x2);
                printf("Enter Y2: ");
                scanf("%d", &y2);
                
                if (!validateCoordinates(x1, y1) || !validateCoordinates(x2, y2)) {
                    printf("\n✗ Coordinates out of bounds! Valid range: X(0-%d), Y(0-%d)\n",
                           CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1);
                } else {
                    addObject(RECTANGLE, x1, y1, x2, y2, 0, 0, 0);
                }
                break;
                
            case 4:
                // Draw Triangle
                printf("\n--- Draw Triangle ---\n");
                printf("Enter X1: ");
                scanf("%d", &x1);
                printf("Enter Y1: ");
                scanf("%d", &y1);
                printf("Enter X2: ");
                scanf("%d", &x2);
                printf("Enter Y2: ");
                scanf("%d", &y2);
                printf("Enter X3: ");
                scanf("%d", &x3);
                printf("Enter Y3: ");
                scanf("%d", &y3);
                
                if (!validateCoordinates(x1, y1) || !validateCoordinates(x2, y2) ||
                    !validateCoordinates(x3, y3)) {
                    printf("\n✗ Coordinates out of bounds! Valid range: X(0-%d), Y(0-%d)\n",
                           CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1);
                } else {
                    addObject(TRIANGLE, x1, y1, x2, y2, x3, y3, 0);
                }
                break;
                
            case 5:
                // Draw Circle
                printf("\n--- Draw Circle ---\n");
                printf("Enter Center X: ");
                scanf("%d", &x1);
                printf("Enter Center Y: ");
                scanf("%d", &y1);
                printf("Enter Radius: ");
                scanf("%d", &radius);
                
                if (!validateCoordinates(x1, y1)) {
                    printf("\n✗ Center coordinates out of bounds!\n");
                } else if (radius < 1 || radius > 15) {
                    printf("\n✗ Invalid radius! Recommended range: 1-15\n");
                } else {
                    addObject(CIRCLE, x1, y1, 0, 0, 0, 0, radius);
                }
                break;
                
            case 6:
                // Delete Object
                printf("\n--- Delete Object ---\n");
                listObjects();
                printf("Enter Object ID to delete: ");
                scanf("%d", &object_id);
                deleteObject(object_id);
                break;
                
            case 7:
                // Modify Object
                printf("\n--- Modify Object ---\n");
                listObjects();
                printf("Enter Object ID to modify: ");
                scanf("%d", &object_id);
                modifyObject(object_id);
                break;
                
            case 8:
                // List All Objects
                listObjects();
                break;
                
            case 9:
                // Clear Canvas
                printf("\nAre you sure? (y/n): ");
                char confirm;
                scanf(" %c", &confirm);
                if (confirm == 'y' || confirm == 'Y') {
                    clearCanvas();
                    canvas.total_objects = 0;
                    printf("✓ Canvas and objects cleared!\n");
                } else {
                    printf("✗ Operation cancelled.\n");
                }
                break;
                
            case 10:
                // Save Canvas
                printf("\n--- Save Canvas ---\n");
                printf("Enter filename: ");
                scanf("%s", filename);
                saveCanvasToFile(filename);
                break;
                
            case 11:
                // Load Canvas
                printf("\n--- Load Canvas ---\n");
                printf("Enter filename: ");
                scanf("%s", filename);
                loadCanvasFromFile(filename);
                break;
                
            case 0:
                // Exit
                printf("\n╔════════════════════════════════════════╗\n");
                printf("║  Thank you for using 2D Graphics Editor ║\n");
                printf("║  Goodbye!                              ║\n");
                printf("╚════════════════════════════════════════╝\n\n");
                exit(0);
                break;
                
            default:
                printf("\n✗ Invalid choice! Please enter 0-11.\n");
        }
    }
    
    return 0;
}
