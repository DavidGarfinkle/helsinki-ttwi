extern crate itertools;
use itertools::Itertools;

pub struct Score {
    pub vectors: Vec<[Note; 2]>,
}

pub struct Note {
    pub onset: f32,
    pub offset: f32,
    pub pitchB40: i32,
}

impl From<Vec<Note>> for Score {
    fn from(notes: Vec<Note>) -> Self {
        let mut vecs: Vec<[Note; 2]>;
        for window in notes.windows(15) {
            for v in window.iter().combinations(2) {
                new_vec = [Note; 2];
            }
        }
        Self {
            vectors: vecs
        }
    }
}

fn main() {
    println!("Hello, world!");
}

/*
        for window in notes.windows(15) {
            for (i, left_note) in window.iter().enumerate() {
                for right_note in &window.iter().
                    vecs.push([left_note, right_note]);
                }
            }
        }
        */