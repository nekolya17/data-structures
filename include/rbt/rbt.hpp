#pragma once
#include <concepts>
#include <utility>
#include <cassert>

namespace ds{

// RBT - Red-Black Tree
/* Usage:
    Collections and dictionaries in STL, java, C#, etc.
    Linux kernel: proccesses scheduler
    When required faster insert and delete than AVL but still fast search
*/
// tests: ctest --test-dir build --output-on-failure 

template <typename T>
concept comparable = requires(const T& a, const T& b){
    {a > b} -> std::convertible_to<bool>;
    {a < b} -> std::convertible_to<bool>;
    {a == b} -> std::convertible_to<bool>;
};


template <comparable T>
class RBT{
public:
    struct TreeNode{
        bool is_red = false;
        T data{};
        TreeNode* parent = nullptr;
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;

        TreeNode(bool red, const T& value, TreeNode* pnt, TreeNode* l, TreeNode* r) : is_red(red), data(value), parent(pnt), left(l), right(r){}                                               
        TreeNode(bool red, T&& value) : is_red(red), data(std::move(value)){}
        TreeNode(bool red, const T& value) : is_red(red), data(value){}
        TreeNode() = default;
    };

private:
    TreeNode* nil_ = nullptr;
    TreeNode* root_ = nullptr;
    size_t size_ = 0;

public: 
    RBT(){
        root_ = nil_ = init_nil();
    }
    ~RBT(){
        if(nil_){
            clear();
            delete nil_;
        }
    }
    RBT(const RBT& other){
        this->nil_ = init_nil();
        this->root_ = copy_tree(other.root_, this->nil_, other.nil_);
        this->size_ = other.size();
    }
    RBT& operator=(const RBT& other){
        if (this == &other) {
            return *this;
        }

        clear();
        delete nil_;

        this->nil_ = init_nil();
        this->root_ = copy_tree(other.root_, this->nil_, other.nil_);
        this->size_ = other.size();
        return *this;
    }
    RBT(RBT&& other) noexcept{
        root_ = nil_ = init_nil();
        size_ = 0;

        std::swap(root_, other.root_);
        std::swap(nil_, other.nil_);
        std::swap(size_, other.size_);
    }
    RBT& operator=(RBT&& other) noexcept{
        if (this != &other) {
            clear();
            delete nil_;

            root_ = nil_ = init_nil();
            size_ = 0;

            std::swap(root_, other.root_);
            std::swap(nil_, other.nil_);
            std::swap(size_, other.size_);
        }
        return *this;
    }

    [[nodiscard]] bool empty() const noexcept{
        return size_ == 0;
    }

    [[nodiscard]] size_t size() const noexcept{
        return this->size_;
    }


    void clear() {
        clear(root_);
        root_ = nil_; 
        size_ = 0;
    }

    

    void insert(const T& value){
        TreeNode* current = root_;
        TreeNode* parent_node = nil_;
        while(current != nil_){
            parent_node = current;
            current = value < current->data? current->left : current->right;
        }
        TreeNode* new_node = new TreeNode(true, value, parent_node, nil_, nil_);
        if(parent_node == nil_){
            root_ = new_node;
        }
        else if(value < parent_node->data){
            parent_node->left = new_node;
        }
        else{
            parent_node->right = new_node;
        }
        size_++;
        insert_fixup(new_node);
    }

    bool search(const T& value) const{
        TreeNode* current = root_;
        while(current != nil_){
            if(current->data == value){
                return true;
            }
            else if(current->data < value){
                current = current->right;
            }
            else{
                current = current->left;
            }
        }
        return false;
    }

    bool remove(const T& value) {
        TreeNode* target = root_;
        while (target != nil_) {
            if (value == target->data) {
                break;
            } else if (value < target->data) {
                target = target->left;
            } else {
                target = target->right;
            }
        }
        if (target == nil_) return false;

        TreeNode* moved_node = target;      
        TreeNode* fixup_node = nullptr;     
        bool original_is_red = moved_node->is_red;

        if (target->left == nil_) {
            fixup_node = target->right;
            transplant(target, target->right);
        } 
        else if (target->right == nil_) {
            fixup_node = target->left;
            transplant(target, target->left);
        } 
        else {
            moved_node = minimum(target->right);
            original_is_red = moved_node->is_red;
            fixup_node = moved_node->right;

            if (moved_node->parent == target) {
                fixup_node->parent = moved_node;
            } 
            else {
                transplant(moved_node, moved_node->right);
                moved_node->right = target->right;
                moved_node->right->parent = moved_node;
            }

            transplant(target, moved_node);
            moved_node->left = target->left;
            moved_node->left->parent = moved_node;
            moved_node->is_red = target->is_red;
        }

        delete target;
        size_--;

        if (!original_is_red) {
            delete_fixup(fixup_node);
        }

        return true;
    }

private:
    TreeNode* init_nil(){
        TreeNode* nil = new TreeNode();
        nil->is_red = false;
        nil->left = nil;
        nil->right = nil;
        nil->parent = nil;
        return nil;
    }
    void clear(TreeNode* node){
        if(node == nil_)
            return;
        
        clear(node->left);
        clear(node->right);
        delete node;
    }

    TreeNode* copy_tree(TreeNode* node, TreeNode* node_parent, TreeNode* other_nil){
        if(node == other_nil)
            return this->nil_;
        
        TreeNode* new_node = new TreeNode(node->is_red, node->data);
        new_node->parent = node_parent;
        new_node->left = copy_tree(node->left, new_node, other_nil);
        new_node->right = copy_tree(node->right, new_node, other_nil);
        return new_node;
    }

    void insert_fixup(TreeNode* node){
        while(node->parent->is_red){
            if(node->parent->parent->left == node->parent){         // Uncle is right
                TreeNode* uncle = node->parent->parent->right;
                if(uncle->is_red){
                    uncle->is_red = node->parent->is_red = false;
                    node->parent->parent->is_red = true;
                    node = node->parent->parent;
                    continue;
                }
                else{
                    if(node == node->parent->right){
                        node = node->parent;
                        rotate_left(node);
                    }
                    node->parent->is_red = false;
                    node->parent->parent->is_red = true;
                    rotate_right(node->parent->parent);
                }
            }
            else{                                                   // Uncle is left
                TreeNode* uncle = node->parent->parent->left;       
                if(uncle->is_red){
                    uncle->is_red = node->parent->is_red = false;
                    node->parent->parent->is_red = true;
                    node = node->parent->parent;
                    continue;
                }
                else{
                    if(node == node->parent->left){
                        node = node->parent;
                        rotate_right(node);
                    }
                    node->parent->is_red = false;
                    node->parent->parent->is_red = true;
                    rotate_left(node->parent->parent);
                }
            }
        }
        root_->is_red = false;
    }

    void rotate_left(TreeNode* x){
        assert(x != nil_ && x->right != nil_);

        TreeNode* tmp = x->right;
        x->right = tmp->left;
        if(tmp->left != nil_){
            tmp->left->parent = x;
        }
        tmp->left = x;
        tmp->parent = x->parent;
        if(tmp->parent == nil_){
            root_ = tmp;
        }
        else if(tmp->parent->left == x){
            tmp->parent->left = tmp;
        }
        else {
            tmp->parent->right = tmp;
        }
        x->parent = tmp;
    }
    void rotate_right(TreeNode* x){
        assert(x != nil_ && x->left != nil_);

        TreeNode* tmp = x->left;
        x->left = tmp->right;
        if(tmp->right != nil_){
            tmp->right->parent = x;
        }
        tmp->right = x;
        tmp->parent = x->parent;
        if(tmp->parent == nil_){
            root_ = tmp;
        }
        else if(tmp->parent->left == x){
            tmp->parent->left = tmp;
        }
        else {
            tmp->parent->right = tmp;
        }
        x->parent = tmp;
    }

    void transplant(TreeNode* u, TreeNode* v) {
        if (u->parent == nil_) {
            root_ = v;
        } else if (u->parent->left == u) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    TreeNode* minimum(TreeNode* node){
        while(node->left != nil_){
            node = node->left;
        }
        return node;
    }

    void delete_fixup(TreeNode* node){
        while (node != root_ && !node->is_red)
        {
            if(node->parent->left == node){
                TreeNode* sibling = node->parent->right;
                if(sibling->is_red){
                    sibling->is_red = false;
                    node->parent->is_red = true;
                    rotate_left(node->parent);
                    sibling = node->parent->right;
                }
                if (!sibling->left->is_red && !sibling->right->is_red) {
                    sibling->is_red = true;
                    node = node->parent;
                } else {
                    if (!sibling->right->is_red) {
                        sibling->left->is_red = false;
                        sibling->is_red = true;
                        rotate_right(sibling);
                        sibling = node->parent->right;
                    }
                    sibling->is_red = node->parent->is_red;
                    node->parent->is_red = false;
                    sibling->right->is_red = false;
                    rotate_left(node->parent);
                    node = root_; 
                }
            }
            else{
                TreeNode* sibling = node->parent->left;
                if(sibling->is_red){
                    sibling->is_red = false;
                    node->parent->is_red = true;
                    rotate_right(node->parent);
                    sibling = node->parent->left;
                }
                if (!sibling->left->is_red && !sibling->right->is_red) {
                    sibling->is_red = true;
                    node = node->parent;
                } else {
                    if (!sibling->left->is_red) {
                        sibling->right->is_red = false;
                        sibling->is_red = true;
                        rotate_left(sibling);
                        sibling = node->parent->left;
                    }
                    sibling->is_red = node->parent->is_red;
                    node->parent->is_red = false;
                    sibling->left->is_red = false;
                    rotate_right(node->parent);
                    node = root_; 
                }
            }
        }
        node->is_red = false;
    }
};


}