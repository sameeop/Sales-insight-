#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define COLOR_LEN 20
#define LOC_LEN 20

typedef struct Product {
    int id;
    char name[NAME_LEN];
    float weight;
    char color[COLOR_LEN];
    char location[LOC_LEN];
    int unitsSold;
    float price;

    int pastSales[5];   // last 5 months
    int psCount;        // how many entries are filled

    int height;
    struct Product *left, *right;
} Product;

// ---------------- AVL HELPERS --------------------
int max(int a, int b) { return (a > b) ? a : b; }
int height(Product* n) { return n ? n->height : 0; }

Product* rightRotate(Product* y) {
    Product* x = y->left;
    Product* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

Product* leftRotate(Product* x) {
    Product* y = x->right;
    Product* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int getBalance(Product* n) {
    if (!n) return 0;
    return height(n->left) - height(n->right);
}

// ---------------- CREATE PRODUCT --------------------
Product* createProduct(int id, const char* name, float weight, const char* color, const char* location, int units, float price) {
    Product* p = (Product*)malloc(sizeof(Product));

    p->id = id;
    strcpy(p->name, name);
    p->weight = weight;
    strcpy(p->color, color);
    strcpy(p->location, location);
    p->unitsSold = units;
    p->price = price;

    // past sales initialization
    for (int i = 0; i < 5; i++) p->pastSales[i] = -1;
    p->psCount = 0;

    p->left = p->right = NULL;
    p->height = 1;
    return p;
}

// ---------------- INSERT (AVL) --------------------
Product* insertAVL(Product* node, int id, const char* name, float weight, const char* color, const char* location, int units, float price) {
    if (!node) return createProduct(id, name, weight, color, location, units, price);

    if (id < node->id)
        node->left = insertAVL(node->left, id, name, weight, color, location, units, price);
    else if (id > node->id)
        node->right = insertAVL(node->right, id, name, weight, color, location, units, price);
    else {
        printf("Product ID already exists.\n");
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));

    int bal = getBalance(node);

    // LL
    if (bal > 1 && id < node->left->id) return rightRotate(node);

    // RR
    if (bal < -1 && id > node->right->id) return leftRotate(node);

    // LR
    if (bal > 1 && id > node->left->id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // RL
    if (bal < -1 && id < node->right->id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// ---------------- SEARCH --------------------
Product* search(Product* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root;
    if (id < root->id) return search(root->left, id);
    return search(root->right, id);
}

// ---------------- DELETE --------------------
Product* minNode(Product* node) {
    Product* curr = node;
    while (curr->left) curr = curr->left;
    return curr;
}

Product* deleteAVL(Product* root, int id) {
    if (!root) return root;

    if (id < root->id)
        root->left = deleteAVL(root->left, id);
    else if (id > root->id)
        root->right = deleteAVL(root->right, id);
    else {
        if (!root->left || !root->right) {
            Product* temp = root->left ? root->left : root->right;
            if (!temp) { temp = root; root = NULL; }
            else *root = *temp;
            free(temp);
        }
        else {
            Product* temp = minNode(root->right);
            root->id = temp->id;
            strcpy(root->name, temp->name);
            root->weight = temp->weight;
            strcpy(root->color, temp->color);
            strcpy(root->location, temp->location);
            root->unitsSold = temp->unitsSold;
            root->price = temp->price;

            for (int i = 0; i < 5; i++) root->pastSales[i] = temp->pastSales[i];
            root->psCount = temp->psCount;

            root->right = deleteAVL(root->right, temp->id);
        }
    }

    if (!root) return root;

    root->height = 1 + max(height(root->left), height(root->right));
    int bal = getBalance(root);

    if (bal > 1 && getBalance(root->left) >= 0) return rightRotate(root);
    if (bal > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (bal < -1 && getBalance(root->right) <= 0) return leftRotate(root);
    if (bal < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// ---------------- DISPLAY --------------------
void displayHeader() {
    printf("-------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-6s | %-8s | %-8s | %-5s | %-7s |\n",
           "ID", "Name", "Wght", "Color", "Location", "Sold", "Price");
    printf("-------------------------------------------------------------------------------\n");
}

void displayProduct(Product* p) {
    printf("| %-5d | %-20s | %-6.2f | %-8s | %-8s | %-5d | $%-6.2f |\n",
           p->id, p->name, p->weight, p->color, p->location, p->unitsSold, p->price);
}

void inorder(Product* root) {
    if (!root) return;
    inorder(root->left);
    displayProduct(root);
    inorder(root->right);
}

// ---------------- ANALYSIS --------------------
void addPastSale(Product* p, int sale) {
    if (p->psCount < 5) {
        p->pastSales[p->psCount++] = sale;
    } else {
        for (int i = 1; i < 5; i++)
            p->pastSales[i - 1] = p->pastSales[i];
        p->pastSales[4] = sale;
    }
}

int predictSales(Product* p) {
    if (p->psCount == 0) return -1;

    int sum = 0;
    for (int i = 0; i < p->psCount; i++)
        sum += p->pastSales[i];

    return sum / p->psCount;
}

void totalRevenue(Product* root, double* total) {
    if (!root) return;
    *total += root->unitsSold * root->price;
    totalRevenue(root->left, total);
    totalRevenue(root->right, total);
}

void bestSeller(Product* root, Product** best) {
    if (!root) return;
    if (!*best || root->unitsSold > (*best)->unitsSold) *best = root;
    bestSeller(root->left, best);
    bestSeller(root->right, best);
}

void leastSeller(Product* root, Product** least) {
    if (!root) return;
    if (!*least || root->unitsSold < (*least)->unitsSold) *least = root;
    leastSeller(root->left, least);
    leastSeller(root->right, least);
}

void analyzeLocation(Product* root, const char* loc, int* units, double* rev) {
    if (!root) return;
    if (strcmp(root->location, loc) == 0) {
        *units += root->unitsSold;
        *rev += root->unitsSold * root->price;
    }
    analyzeLocation(root->left, loc, units, rev);
    analyzeLocation(root->right, loc, units, rev);
}

// ---------------- FILE HANDLING --------------------
void saveToFile(Product* root, FILE* fp) {
    if (!root) return;

    fprintf(fp, "%d,%s,%.2f,%s,%s,%d,%.2f,%d,%d,%d,%d,%d,%d\n",
            root->id, root->name, root->weight, root->color, root->location,
            root->unitsSold, root->price,
            root->pastSales[0], root->pastSales[1], root->pastSales[2],
            root->pastSales[3], root->pastSales[4], root->psCount);

    saveToFile(root->left, fp);
    saveToFile(root->right, fp);
}

Product* loadData() {
    FILE* fp = fopen("sales_data.txt", "r");
    if (!fp) return NULL;

    Product* root = NULL;
    int id, units, ps0, ps1, ps2, ps3, ps4, psCount;
    char name[50], color[20], loc[20];
    float weight, price;

    while (fscanf(fp, "%d,%49[^,],%f,%19[^,],%19[^,],%d,%f,%d,%d,%d,%d,%d,%d\n",
                  &id, name, &weight, color, loc, &units, &price,
                  &ps0, &ps1, &ps2, &ps3, &ps4, &psCount) == 13) {

        root = insertAVL(root, id, name, weight, color, loc, units, price);

        Product* p = search(root, id);
        p->pastSales[0] = ps0;
        p->pastSales[1] = ps1;
        p->pastSales[2] = ps2;
        p->pastSales[3] = ps3;
        p->pastSales[4] = ps4;
        p->psCount = psCount;
    }

    fclose(fp);
    return root;
}

void saveData(Product* root) {
    FILE* fp = fopen("sales_data.txt", "w");
    saveToFile(root, fp);
    fclose(fp);
}

// ---------------- UTIL --------------------
void readLine(char* buf, int size) {
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

int readInt() {
    char buf[50];
    readLine(buf, 50);
    return atoi(buf);
}

float readFloat() {
    char buf[50];
    readLine(buf, 50);
    return atof(buf);
}

// ---------------- MAIN MENU --------------------
int main() {
    Product* root = loadData();
    int choice;

    while (1) {
        printf("\n=== SALES INSIGHT (AVL + Prediction) ===\n");
        printf("1. Add New Product\n");
        printf("2. Search Product by ID\n");
        printf("3. Display All Products\n");
        printf("4. Update Product Sales\n");
        printf("5. Delete Product\n");
        printf("6. View Insights\n");
        printf("7. Analyze by Location\n");
        printf("8. Predict Sales of Product\n");
        printf("9. Save & Exit\n");
        printf("Enter choice: ");

        choice = readInt();

        if (choice == 1) {
            int id, units;
            float w, pr;
            char name[50], color[20], loc[20];

            printf("ID: "); id = readInt();
            printf("Name: "); readLine(name, 50);
            printf("Weight: "); w = readFloat();
            printf("Color: "); readLine(color, 20);
            printf("Location: "); readLine(loc, 20);
            printf("Units Sold: "); units = readInt();
            printf("Price: "); pr = readFloat();

            root = insertAVL(root, id, name, w, color, loc, units, pr);
        }

        else if (choice == 2) {
            printf("Enter Product ID: ");
            int id = readInt();
            Product* p = search(root, id);
            if (!p) printf("Not found.\n");
            else { displayHeader(); displayProduct(p); }
        }

        else if (choice == 3) {
            displayHeader();
            inorder(root);
        }

        else if (choice == 4) {
            printf("Enter Product ID: ");
            int id = readInt();
            Product* p = search(root, id);

            if (!p) printf("Not found.\n");
            else {
                printf("Enter new units sold: ");
                p->unitsSold = readInt();

                printf("Enter past month sales value to store: ");
                int ps = readInt();
                addPastSale(p, ps);

                printf("Updated.\n");
            }
        }

        else if (choice == 5) {
            printf("Enter ID to delete: ");
            int id = readInt();
            root = deleteAVL(root, id);
        }

        else if (choice == 6) {
            double total = 0;
            Product *best = NULL, *least = NULL;

            totalRevenue(root, &total);
            bestSeller(root, &best);
            leastSeller(root, &least);

            printf("Total Revenue: %.2f\n", total);
            if (best) printf("Best Seller: %s (%d units)\n", best->name, best->unitsSold);
            if (least) printf("Least Seller: %s (%d units)\n", least->name, least->unitsSold);
        }

        else if (choice == 7) {
            char loc[20];
            printf("Enter Location: ");
            readLine(loc, 20);

            int units = 0; double rev = 0;
            analyzeLocation(root, loc, &units, &rev);

            printf("Location: %s | Units: %d | Revenue: %.2f\n", loc, units, rev);
        }

        else if (choice == 8) {
            printf("Enter Product ID: ");
            int id = readInt();
            Product* p = search(root, id);

            if (!p) printf("Not found.\n");
            else {
                int pred = predictSales(p);
                if (pred == -1) printf("Not enough past data.\n");
                else printf("Predicted next-month sales: %d units\n", pred);
            }
        }

        else if (choice == 9) {
            saveData(root);
            printf("Saved. Exiting.\n");
            break;
        }

        else printf("Invalid choice.\n");
    }

    return 0;
}
