open Framework;
open QCheckRely;
open Generator.Fantasy;
open Lib.Troll;

let {describe} = extendDescribe(QCheckRely.Matchers.matchers);

/*
 * Assertions below assumed that 'same troll' means a trolls with the same score
 * This is a valid assertion regarding the use of the app (compare scoring)
 * If you want assert that same trolls means same record there is still a final bug to find :)
 * There is to way to represent "no scoring for this kind of elf"
 *   -> If you never killed a kind of elf there is no key for that elf in Troll.t.Killed map
 *   -> If you killed an amount of a kind of elf and the same amount ressurect you will have a key for that elf in Troll.t.Killed map with a value of 0
 */

describe("Troll Invariance", ({test}) => {
  test("Troll score should be 0 when all elves resurrected", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="Troll score should be 0 when all elves resurrected",
      troll_arbitrary,
      troll =>
      all_elves_resurrected(troll) |> scoring == 0
    )
    |> expect.ext.qCheckTest;
    ();
  });
  test("Troll score should always be >= 0", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="Troll score should always be >= 0",
      troll_arbitrary,
      troll =>
      scoring(troll) >= 0
    )
    |> expect.ext.qCheckTest;
    ();
  });
});

describe("Troll Inverse", ({test}) => {
  test("oops_he_survived should always be inverse of i_got_one", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="oops_he_survived should always be inverse of i_got_one",
      troll_elf_arbitrary,
      ((troll, elf)) =>
      i_got_one(elf, troll)
      |> oops_he_survived(elf)
      |> scoring == scoring(troll)
    )
    |> expect.ext.qCheckTest;
    ();
  })
});

describe("Troll Analogy", ({test}) => {
  test("i_got_one and i_got should be consistent", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="i_got_one and i_got should be consistent",
      troll_elf_int_arbitrary,
      ((troll, elf, qty)) =>
      i_got(qty, elf, troll)
      == (
           List.init(qty, _ => 1)  // alternative to a for loop : fill a list of 1 and fold on it
           |> List.fold_left(
                (cur_troll, _) => i_got_one(elf, cur_troll),
                troll,
              )
         )
    )
    |> expect.ext.qCheckTest;
    ();
  })
});

describe("Troll Idempotence", ({test}) => {
  test(
    "all_elves_of_a_kind_resurrected brings the Troll killing list to a stable state",
    ({expect}) => {
      QCheck.Test.make(
        ~count=1000,
        ~name=
          "all_elves_of_a_kind_resurrected brings the Troll killing list to a stable state",
        troll_elf_int_arbitrary,
        ((troll, elf, times)) =>
        if (times > 0) {
          all_elves_of_a_kind_resurrected(elf, troll)
          == (
               List.init(times, _ => 1)  // alternative to a for loop : fill a list of 1 and fold on it
               |> List.fold_left(
                    (cur_troll, _) =>
                      all_elves_of_a_kind_resurrected(elf, cur_troll),
                    troll,
                  )
             );
        } else {
          true; // idempotence don't means anything 0 times ... Stats on 1000 tests make this assertion good enough
        }
      )
      |> expect.ext.qCheckTest;
      ();
    },
  )
});

describe("Troll Metamorphism", ({test}) => {
  test(
    "For any troll and any elf i_got_one should increase troll scoring",
    ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name=
        "For any troll and any elf i_got_one should increase troll scoring",
      troll_elf_arbitrary,
      ((troll, elf)) =>
      i_got_one(elf, troll) |> scoring > scoring(troll)
    )
    |> expect.ext.qCheckTest;
    ();
  });
  test(
    "For any troll and any elf all_elves_of_a_kind_resurrected should decrease troll scoring",
    ({expect}) => {
      QCheck.Test.make(
        ~count=1000,
        ~name=
          "For any troll and any elf all_elves_of_a_kind_resurrected should decrease troll scoring",
        troll_elf_arbitrary,
        ((troll, elf)) =>
        all_elves_of_a_kind_resurrected(elf, troll)
        |> scoring <= scoring(troll)
      )
      |> expect.ext.qCheckTest;
      ();
    },
  );
});

describe("Troll Injection", ({test}) => {
  test(
    "For any Troll and any 2 diff elves, troll after killing elf1 != Troll after killing elf2",
    ({expect}) => {
      QCheck.Test.make(
        ~count=1000,
        ~name=
          "For any troll and any elf i_got_one should increase troll scoring",
        troll_two_elves_arbitrary,
        ((troll, elf1, elf2)) =>
        if (elf1 != elf2) {
          i_got_one(elf1, troll) != i_got_one(elf2, troll);
        } else {
          i_got_one(elf1, troll) == i_got_one(elf2, troll);
        }
      )
      |> expect.ext.qCheckTest;
      ();
    },
  )
});